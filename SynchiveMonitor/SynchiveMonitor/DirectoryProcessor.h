#pragma once
#include "stdafx.h"
#include "FileProcessorBase.h"

ref class DirectoryProcessor : public FileProcessorBase
{
public:
	DirectoryProcessor(String^ path);
	void addFile(String^ id);

	virtual void didProcessFile(String^ id, String^ path, int depth, String^ crc) override;
	virtual void willProcessDirectory(String^ id, String^ path, int depth) override;

	static void fileCreated(Object^ sender, FileSystemEventArgs^ e);
	static void fileDeleted(Object^ sender, FileSystemEventArgs^ e);
	static void fileRenamed(Object^ sender, RenamedEventArgs^ e);
private:
	Hashtable^ fileList; // using hashtable for O(1) specific search and inserting
};

