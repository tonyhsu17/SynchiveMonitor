#include "stdafx.h"
#include "DirectoryManagement.h"


DirectoryManagement::DirectoryManagement(String^ path) : FileProcessorBase(path)
{
	directoryList = gcnew Hashtable();
	processingQueue = gcnew Queue();
	directoryModified = false;
}

void DirectoryManagement::didProcessFile(String^ id, String^ dirID, String^ path, int depth, String^ crc)
{
	Console::WriteLine("@didProcessFile: " + id + " level: " + depth);
	Hashtable^ fileList = (Hashtable^)directoryList[dirID]; // [name, crc]
	FileInfo^ info = gcnew FileInfo(path);
	fileList->Add(info->Name, crc); 
	directoryModified = true;
	delete info;
}

void DirectoryManagement::willProcessDirectory(String^ id, String^ path, int depth)
{
	Console::WriteLine("@willProcessDirectory: " + id);
	Hashtable^ fileList = gcnew Hashtable();

	directoryList->Add(id, fileList);
	directoryModified = true;
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

	int depth = getDepth(path, root->path);
	String^ id = getDirectoryUniqueID(path, depth, root->path);

	if (directoryList->Contains(id))
	{
		// directory found, now remove directory + fileList AND 
		// somehow find subdirectories and remove those too
	}
	else
	{
		Hashtable^ fileList = (Hashtable^)directoryList[id];
		//fileList->Remove("NAME"); // TODO: proper name
	}
}

void DirectoryManagement::fileRenamed(RenamedEventArgs ^ e)
{
	String^ path = e->FullPath;
	String^ oldPath = e->OldFullPath;
	Console::WriteLine("FileRenamed: " + e->FullPath + " from: " + e->OldFullPath);
	if (Directory::Exists(path)) // check if directory or file
	{
		int depth = getDepth(path, root->path);
		String^ id = getDirectoryUniqueID(path, depth, root->path);
		int oldDepth = getDepth(oldPath, root->path);
		String^ oldID = getDirectoryUniqueID(oldPath, depth, root->path);

		Hashtable^ fileList = (Hashtable^)directoryList[oldID];
		directoryList->Add(id, fileList);
		directoryList->Remove(oldID);
		directoryModified = true;

		// TODO: handle sub dirs
	}
	else if(File::Exists(path))
	{
		FileInfo^ info = gcnew FileInfo(path);
		FileInfo^ oldInfo = gcnew FileInfo(oldPath);
		int depth = getDepth(path, root->path);
		String^ dirID = getDirectoryUniqueID(info->Directory->FullName, depth, root->path);

		Hashtable^ fileList = (Hashtable^)directoryList[dirID]; // get fileList
		String^ crc = (String^)fileList[oldInfo->Name]; // get crc
		fileList->Add(e->Name, crc);
		fileList->Remove(e->OldName);
		directoryModified = true;
	}
}

void DirectoryManagement::processQueue()
{
	try
	{
		while (processingQueue->Count > 0)
		{
			Console::WriteLine("@Processing Start");
			String^ path = (String^)processingQueue->Peek();

			if (Directory::Exists(path)) // check if directory or file
			{
				DirectoryInfo^ info = gcnew DirectoryInfo(path);
				int depth = getDepth(path, root->path);
				String^ id = getDirectoryUniqueID(path, depth, root->path);

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

				delete info;
			}
			else if (File::Exists(path))
			{
				FileInfo^ fileInfo = gcnew FileInfo(path);
				String^ crc = calculateCRC32(path);
				if (crc != nullptr) {
					String^ fileID = getFileUniqueID(fileInfo->Name, crc);
					int depth = getDepth(path, root->path);
					String^ dirID = getDirectoryUniqueID(fileInfo->Directory->FullName, depth, root->path);
					Console::WriteLine("@dirID: " + dirID);
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

					Console::WriteLine("@processQueue: " + path);
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
	
	sw->WriteLine("Generated with SynchiveMonitor - root=" + root->path);

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

