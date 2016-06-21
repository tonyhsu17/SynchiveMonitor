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
	Console::WriteLine("FileCreated: " + e->FullPath);
	if (e->Name != kIDFileName)
	{
		processingQueue->Enqueue(e->FullPath);
	}
}

void DirectoryManagement::fileDeleted(FileSystemEventArgs ^ e)
{
	String^ path = e->FullPath;
	Console::WriteLine("FileDeleted: " + path);

	if (Directory::Exists(path)) // check if directory or file
	{
		//DirectoryInfo^ info = gcnew DirectoryInfo(path);
		int depth = getDepth(path, root->path);
		String^ id = getDirectoryUniqueID(path, depth, root->path);
		
		// get all files in directory and remove them
		Hashtable^ fileList = (Hashtable^)directoryList[id]; 
		delete fileList;
		directoryList->Remove(id);
		directoryModified = true;
	}
	else if(File::Exists(path))
	{
		FileInfo^ info = gcnew FileInfo(path);
		int depth = getDepth(path, root->path);
		String^ dirID = getDirectoryUniqueID(info->Directory->FullName, depth, root->path);

		Hashtable^ fileList = (Hashtable^)directoryList[dirID];
		fileList->Remove(info->Name);
		directoryModified = true;
	}
}

void DirectoryManagement::fileRenamed(RenamedEventArgs ^ e)
{
	String^ path = e->FullPath;
	String^ oldPath = e->FullPath;
	Console::WriteLine("FileRenamed: " + e->FullPath + " from: " + e->OldFullPath);
	if (Directory::Exists(path)) // check if directory or file
	{
		int depth = getDepth(path, root->path);
		String^ id = getDirectoryUniqueID(path, depth, root->path);
		int oldDepth = getDepth(oldPath, root->path);
		String^ oldID = getDirectoryUniqueID(oldPath, depth, root->path);

		Hashtable^ fileList = (Hashtable^)directoryList[id];
		directoryList->Add(id, fileList);
		directoryList->Remove(oldID);
		directoryModified = true;
	}
	else if(File::Exists(path))
	{
		FileInfo^ info = gcnew FileInfo(path);
		int depth = getDepth(path, root->path);
		String^ dirID = getDirectoryUniqueID(info->Directory->FullName, depth, root->path);

		Hashtable^ fileList = (Hashtable^)directoryList[dirID];
		String^ crc = (String^)fileList[info->Name];
		fileList->Remove(e->OldName);
		fileList->Add(e->Name, crc);
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
					fileList->Add(fileInfo->Name, crc);
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

