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

void DirectoryProcessor::didProcessFile(String^ id, String^ path, int depth, String^ crc)
{
	Console::WriteLine("@didProcessFile: " + id + " level: " + depth);
	//throw gcnew System::NotImplementedException();
}

void DirectoryProcessor::willProcessDirectory(String^ id, String^ path, int depth)
{
	Console::WriteLine("@willProcessDirectory: " + id);
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


