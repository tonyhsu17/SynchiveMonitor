#include "stdafx.h"
#include "DirectoryManagement.h"


DirectoryManagement::DirectoryManagement(String^ path) : FileProcessorBase(path)
{
	directoryList = gcnew Hashtable();
	processingQueue = gcnew Queue();
	directoryModified = false;
}

// only used to read file, no modifing is done
void DirectoryManagement::didProcessFile(String^ id, String^ dirID, String^ path, int depth, String^ crc)
{
	if(!doesRootIDFileExist())
	{
		directoryModified = true;
	}
	Hashtable^ fileList = (Hashtable^)directoryList[dirID]; // [name, crc]
	FileInfo^ info = gcnew FileInfo(path);
	fileList->Add(info->Name, crc); 
	delete info;
}

// only used to read file, no modifing is done
void DirectoryManagement::willProcessDirectory(String^ id, String^ path, int depth)
{
	if(!doesRootIDFileExist())
	{
		directoryModified = true;
	}
	Hashtable^ fileList = gcnew Hashtable();
	directoryList->Add(id, fileList);
}

void DirectoryManagement::fileCreated(FileSystemEventArgs ^ e)
{
	if (e->Name != kIDFileName)
	{
		Console::WriteLine("FileCreated: " + e->FullPath);
		processingQueue->Enqueue(e->FullPath);
	}
}

void DirectoryManagement::fileDeleted(FileSystemEventArgs ^ e)
{
	String^ path = e->FullPath;
	Console::WriteLine("FileDeleted: " + path);

	DirectoryInfo^ info = gcnew DirectoryInfo(path);
	int depth = getDepth(path, root->path, false);
	String^ id = getDirectoryUniqueID(path, depth, root->path);

	if (directoryList->Contains(id)) // if directory
	{
		Queue^ keysToRemove = gcnew Queue(); // since we can't modify list while enurmating the list, we store in queue
		for each(DictionaryEntry de in directoryList)
		{
			SynchiveDirectory^ dir = getSynchiveDirectory((String^)de.Key, root->path);
			if (dir->path->StartsWith(path))
			{
				Hashtable^ fileList = (Hashtable^)directoryList[(String^)de.Key];
				delete fileList;
				keysToRemove->Enqueue(de.Key);
			}
			delete dir;
		}
		for each(String^ key in keysToRemove)
		{
			directoryList->Remove(key);
		}
		delete keysToRemove;
	}
	else // else file
	{
		FileInfo^ info = gcnew FileInfo(path);
		int depth = getDepth(path, root->path, true);
		String^ id = getDirectoryUniqueID(info->Directory->FullName, depth, root->path);
		Hashtable^ fileList = (Hashtable^)directoryList[id];

		if (fileList != nullptr) // if file
		{
			fileList->Remove(info->Name);
			delete info;
		}
	}
	directoryModified = true;
}


void DirectoryManagement::fileRenamed(RenamedEventArgs ^ e)
{
	Console::WriteLine("FileRenamed to: " + e->FullPath + " from " + e->OldFullPath);
	// spoof a file created event in case of created file not processed yet gets renamed.
	FileSystemEventArgs^ spoofedCreated = gcnew FileSystemEventArgs(WatcherChangeTypes::Created, e->FullPath->Substring(0, e->FullPath->Length - e->Name->Length) , e->Name);
	fileCreated(spoofedCreated);

	String^ path = e->FullPath;
	String^ oldPath = e->OldFullPath;
	if (Directory::Exists(path)) // check if directory or file
	{
		DirectoryInfo^ parent = gcnew DirectoryInfo(path);
		handleDirectoryRename(path, parent->FullName, oldPath);
		delete parent;
	}
	else if(File::Exists(path))
	{
		FileInfo^ info = gcnew FileInfo(path);
		FileInfo^ oldInfo = gcnew FileInfo(oldPath); // able to retrieve name from invalid path
		int depth = getDepth(path, root->path, true);
		String^ dirID = getDirectoryUniqueID(info->Directory->FullName, depth, root->path);

		Hashtable^ fileList = (Hashtable^)directoryList[dirID]; // get fileList
		String^ crc = (String^)fileList[oldInfo->Name]; // get crc
		fileList->Add(info->Name, crc);
		fileList->Remove(oldInfo->Name);
		directoryModified = true;
	}
}

void DirectoryManagement::handleDirectoryRename(String^ path, String^ newBasePath, String^ oldBasePath)
{
	DirectoryInfo^ info = gcnew DirectoryInfo(path); // info of current dir
	array<DirectoryInfo^>^ subDir = info->GetDirectories(); // list of sub dir
	for each(DirectoryInfo^ tempDir in subDir)
	{
		handleDirectoryRename(tempDir->FullName, newBasePath, oldBasePath); // recurse through each sub dir
	}

	String^ oldPath = oldBasePath + getRelativePath(path, newBasePath); // reconstruct original path
	
	int oldDepth = getDepth(path, root->path, false); // depth cannot change from renaming
	String^ oldID = getDirectoryUniqueID(oldPath, oldDepth, root->path); // get originalID
	String^ id = getDirectoryUniqueID(path, oldDepth, root->path); // get newID

	Hashtable^ fileList = (Hashtable^)directoryList[oldID];
	if(fileList != nullptr) 
	{
		// check if directory exist first
		// could be null from a directory created and rename before directory was processed
		directoryList->Add(id, fileList); // add directory with new name
		directoryList->Remove(oldID);
		directoryModified = true;
	}
	delete info;
}

void DirectoryManagement::processQueue()
{
	try
	{
		while (processingQueue->Count > 0)
		{
			String^ path = (String^)processingQueue->Peek();

			if (Directory::Exists(path)) // check if directory or file
			{
				DirectoryInfo^ info = gcnew DirectoryInfo(path);
				int depth = getDepth(path, root->path, false);
				String^ id = getDirectoryUniqueID(path, depth, root->path);
				Hashtable^ test = (Hashtable^)directoryList[id];
				// don't process is directory already exist
				if(!directoryList->Contains(id))
				{
					array<DirectoryInfo^>^ subDirs = info->GetDirectories();
					for each(DirectoryInfo^ d in subDirs)
					{
						processingQueue->Enqueue(d->FullName);
					}
					array<FileInfo^>^ files = info->GetFiles();
					for each(FileInfo^ f in files)
					{
						processingQueue->Enqueue(f->FullName);
					}
				}
				delete info;
			}
			else if (File::Exists(path))
			{
				FileInfo^ fileInfo = gcnew FileInfo(path);
				String^ crc = calculateCRC32(path);
				if (crc != nullptr) {
					String^ fileID = getFileUniqueID(fileInfo->Name, crc);
					int depth = getDepth(path, root->path, true);
					String^ dirID = getDirectoryUniqueID(fileInfo->Directory->FullName, depth, root->path);

					if (!directoryList->Contains(dirID)) //create fileList hashtable
					{
						directoryList->Add(dirID, gcnew Hashtable());
					}
					Hashtable^ fileList = (Hashtable^)directoryList[dirID];
					if (fileList->Contains(fileInfo->Name))
					{
						fileList[fileInfo->Name] = crc;
					}
					else
					{
						fileList->Add(fileInfo->Name, crc);
					}
					directoryModified = true;
				}
				delete fileInfo;
			}
			processingQueue->Dequeue(); // remove from list (not removed initially to account for exceptions)
		}
	}
	catch (IOException^ ioE) // catch file in use
	{
		// wait for next interation to process files
	}
}

void DirectoryManagement::writeToFile()
{
	if (!directoryModified)
	{
		return;
	}
	directoryModified = false;
	Console::WriteLine("@Writting to file");
	String^ idFilePath = root->path + "\\" + kIDFileName;
	StreamWriter^ sw = gcnew StreamWriter(idFilePath, false, Text::Encoding::UTF8);
	
	sw->WriteLine("Generated with SynchiveMonitor {0} - root=" + root->path, kVersion);

	for each(DictionaryEntry de in directoryList)
	{
		sw->WriteLine((String^)de.Key);

		Hashtable^ fileList = (Hashtable^)de.Value;
		for each(DictionaryEntry fileDE in fileList)
		{
			sw->WriteLine((String^)fileDE.Value + " \""+(String^)fileDE.Key + "\"");
		}
	}
	sw->Close();
	delete sw;
	
}

