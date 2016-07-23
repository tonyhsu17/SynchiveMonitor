#include "stdafx.h"
#include "FileProcessorBase.h"


FileProcessorBase::FileProcessorBase(String ^ dirPath)
{
	directoyMapping = gcnew Hashtable();
	try
	{
		//reader = gcnew StreamReader(path);
	}
	catch(Exception^ e)
	{
		Console::WriteLine(e->Message);
	}
	root = gcnew SynchiveDirectory();
	root->path = dirPath;
	root->depth = 0;

	directoriesToProcess = gcnew Stack();
	directoriesToProcess->Push(root);
}



void FileProcessorBase::readinIDs()
{
	Console::WriteLine("@readinIDs");
	while (directoriesToProcess->Count > 0)
	{
		SynchiveDirectory^ dir = (SynchiveDirectory^)directoriesToProcess->Pop();
		array<String^>^ idFiles = Directory::GetFiles(dir->path, kIDFileName); // filter out every file except idFile

		// if idFile not found, process each file within directory
		if (idFiles->Length == 0)
		{
			String^ dirID = getDirectoryUniqueID(dir->path, dir->depth, root->path);
			willProcessDirectory(dirID, dir->path, dir->depth);
			readFilesWithinDirectory(dir);
		}
		else
		{
			try
			{
				// if current file is a subIDFile
				readFromIDFile(idFiles[0], dir->depth);
			}
			catch (InvalidOperationException^ e)
			{
				Console::WriteLine(e->Message);
			}
		}

		// if dir != root, delete
		if (dir != root)
		{
			delete dir;
		}
	}
}


void FileProcessorBase::readFilesWithinDirectory(SynchiveDirectory^ dir)
{
	for each (String^ subDir in Directory::GetDirectories(dir->path))
	{
		// there is no need to skip folder as the copier will exclude them
		SynchiveDirectory^ tempDir = gcnew SynchiveDirectory;
		tempDir->path = subDir;
		tempDir->depth = dir->depth + 1;
		directoriesToProcess->Push(tempDir);
	}
	for each (String^ file in Directory::GetFiles(dir->path))
	{
		// create new file entry
		if (file != kIDFileName)
		{
			String^ dirID = getDirectoryUniqueID(dir->path, dir->depth, root->path);
			FileInfo^ info = gcnew FileInfo(file);
			String^ crcVal = calculateCRC32(file);
			String^ fileID = getFileUniqueID(info->Name, crcVal);

			didProcessFile(fileID, dirID, file, dir->depth, crcVal);
			delete info;
		}
	}
}

void FileProcessorBase::readFromIDFile(String ^ path, int baseDepth)
{
	Console::WriteLine("@readFromIDFile");
	StreamReader^ sc = gcnew StreamReader(path);
	String^ str = sc->ReadLine(); // strip out header
	if (str == nullptr) // in-case of empty file
	{
		sc->Close();
		delete sc;
		throw gcnew System::InvalidOperationException("Empty File");
	}

	SynchiveFile file;
	file.path = path;
	file.depth = baseDepth;

	Console::WriteLine(path);
	Console::WriteLine(Directory::GetParent(file.path)->ToString());

	String^ locationDir = Directory::GetParent(file.path)->ToString(); // directory of root

	array<wchar_t>^ filter = gcnew array<wchar_t>(1); // required otherwise split returns more than max substrings
	filter[0] = ' ';

	str = sc->ReadLine();
	while (str != nullptr && str->StartsWith(kDirLinePrefix)) // not finished and is a folder
	{
		array<String^>^ splitDir = str->Split(filter, 2); // [level, path]

		if (splitDir->Length != 2)
		{
			sc->Close();
			delete sc;
			throw gcnew System::InvalidOperationException("Bad format found");
		}

		int newLevel = Int32::Parse(splitDir[0]->Substring(1, 1)) + baseDepth;

		String^ path = locationDir + splitDir[1];
		String^ dirID = getDirectoryUniqueID(path, newLevel, root->path);

		willProcessDirectory(dirID, path, newLevel);

		str = sc->ReadLine();
		while (str != nullptr && !str->StartsWith(kDirLinePrefix))
		{
			array<String^>^ splitStr = str->Split(filter, 2); // [crc, name]

			if (splitStr->Length != 2)
			{
				sc->Close();
				delete sc;
				throw gcnew System::InvalidOperationException("Bad format found");
			}
			// reconstruct file path (root path + directory path + fileName)
			String^ fileLoc = locationDir + splitDir[1] + "\\" +
				splitStr[1]->Substring(1, splitStr[1]->Length - 2); // null terminated counted?
			FileInfo^ info = gcnew FileInfo(fileLoc);
			didProcessFile(getFileUniqueID(info->Name, splitStr[0]), dirID, fileLoc, newLevel, splitStr[0]); // abstract method
			delete info;
			str = sc->ReadLine();
		}
	}
	sc->Close();
	delete sc;
	delete filter;
}

String^ FileProcessorBase::getFileUniqueID(String^ name, String^ crc)
{
	return crc + " \"" + name + "\"";
}

FileProcessorBase::SynchiveDirectory ^ FileProcessorBase::getSynchiveDirectory(String ^ id, String^ root)
{
	array<wchar_t>^ filter = gcnew array<wchar_t>(1); // required otherwise split returns more than max substrings
	filter[0] = ' ';
	array<String^>^ splitDir = id->Split(filter, 2); // [level, path]
	
	if (splitDir->Length != 2)
	{
		return nullptr;
	}

	SynchiveDirectory^ dir = gcnew SynchiveDirectory;
	dir->depth = Int32::Parse(splitDir[0]->Substring(1, 1));
	dir->path = root + splitDir[1];
	delete filter;
	return dir;
}

FileProcessorBase::SynchiveFile ^ FileProcessorBase::getSynchiveFile(String ^ id, String^ parent)
{
	array<wchar_t>^ filter = gcnew array<wchar_t>(1); // required otherwise split returns more than max substrings
	filter[0] = ' ';
	array<String^>^ splitStr = id->Split(filter, 2); // [crc, name]
	

	if (splitStr->Length != 2)
	{
		return nullptr;
	}

	SynchiveFile^ file = gcnew SynchiveFile;
	file->crc = splitStr[0];
	file->path = parent + "\\" +
		splitStr[1]->Substring(1, splitStr[1]->Length - 2); // null terminated counted? ;
	
	delete filter;
	return file;
}


// Get the UniqueID of a directory.
String ^ FileProcessorBase::getDirectoryUniqueID(String ^ filePath, int level, String ^ rootPath)
{
	return "~" + level + ": " + filePath->Substring(rootPath->Length);
}


String ^ FileProcessorBase::calculateCRC32(String ^ file)
{
	CRC32^ crc = gcnew CRC32();
	String^ crcVal;
	try
	{
		crcVal = crc->computeHash(file);
	}
	catch (IOException^ e)
	{
		delete crc;
		throw e;
	}
	
	delete crc;
	return crcVal;
}

int FileProcessorBase::getDepth(String ^ path, String ^ root, Boolean isFile)
{
	String^ relativePath = path->Substring(root->Length);
	array<String^>^ splitPath = relativePath->Split('\\');
	int depth = splitPath->Length - (isFile ? 1 : 0); // subtract one if file since depth is 1 less than path
	for each(String^ s in splitPath)
	{
		if (s->Length == 0) // s->Empty returning true for everything....
		{
			depth--; // strip out empty ones
		}
	}
	//Console::WriteLine("@depth: " + depth + " - path: " + path + " root: " + root + " isFile: " + isFile);
	return depth;
}

String^ FileProcessorBase::getRelativePath(String^ path, String^ basePath)
{
	
	return path->Substring(basePath->Length);
}

bool FileProcessorBase::doesRootIDFileExist()
{
	return File::Exists(root->path);
}
