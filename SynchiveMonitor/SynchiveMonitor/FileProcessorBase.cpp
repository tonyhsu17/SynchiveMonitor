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

	root.path = dirPath;
	root.depth = 0;

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
			String^ dirID = getDirectoryUniqueID(dir->path, dir->depth, root.path);
			willProcessDirectory(dir);
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
	}


	/*String^ line;

	line = reader->ReadLine();
	while (line != nullptr)
	{

		line = reader->ReadLine();
	}*/
}

// Get the UniqueID of a directory.
String ^ FileProcessorBase::getDirectoryUniqueID(String ^ filePath, int level, String ^ rootPath)
{
	return "~" + level + ": " + filePath->Substring(rootPath->Length);
}

void FileProcessorBase::readFilesWithinDirectory(SynchiveDirectory^ dir)
{
	throw gcnew System::NotImplementedException();
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

	str = sc->ReadLine();
	while (str != nullptr && str->StartsWith(kDirLinePrefix)) // not finished and is a folder
	{
		array<wchar_t>^ filter = gcnew array<wchar_t>(1);
		filter[0] = ' ';
		array<String^>^ splitDir = str->Split(filter, 2); // [level, path]

		if (splitDir->Length != 2)
		{
			sc->Close();
			delete sc;
			throw gcnew System::InvalidOperationException("Bad format found");
		}

		int newLevel = Int32::Parse(splitDir[0]->Substring(1, 1)) + baseDepth;

		String^ path = locationDir + splitDir[1];
		String^ dirID = getDirectoryUniqueID(path, newLevel, root.path);

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
				splitStr[1]->Substring(1, splitStr[1]->Length - 1);
			
			didProcessFile(fileLoc, newLevel, splitStr[0]); // abstract method
			str = sc->ReadLine();
		}
	}
	sc->Close();
	delete sc;
}
