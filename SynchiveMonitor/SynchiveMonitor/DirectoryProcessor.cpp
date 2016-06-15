#include "stdafx.h"
#include "DirectoryProcessor.h"


DirectoryProcessor::DirectoryProcessor(String^ path) : FileProcessorBase(path)
{
	fileList = gcnew Hashtable();
}

void DirectoryProcessor::addFile(String ^ id)
{
	fileList->Add(id, 1);
}

void DirectoryProcessor::didProcessFile(String^ filePath, int level, String^ crc)
{
	Console::WriteLine("@didProcessFile: " + filePath + " level: " + level + " crc: " + crc);
	//throw gcnew System::NotImplementedException();
}

void DirectoryProcessor::willProcessDirectory(SynchiveDirectory^ dir)
{
	Console::WriteLine("@willProcessDirectory: " + dir->path + " level: " + dir->depth);
	//throw gcnew System::NotImplementedException();
}

void DirectoryProcessor::fileCreated(Object ^ sender, FileSystemEventArgs ^ e)
{
	Console::WriteLine("FileCreated: " + e->FullPath);

}

void DirectoryProcessor::fileDeleted(Object ^ sender, FileSystemEventArgs ^ e)
{
	Console::WriteLine("FileDeleted: " + e->FullPath);
}

void DirectoryProcessor::fileRenamed(Object ^ sender, RenamedEventArgs ^ e)
{
	Console::WriteLine("FileRenamed: " + e->FullPath + " from: " + e->OldFullPath);
}


