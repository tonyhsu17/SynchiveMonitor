#pragma once
#include "stdafx.h"
#include "FileProcessorBase.h"

ref class DirectoryManagement : public FileProcessorBase
{
public:
	DirectoryManagement(String^ path);

	virtual void didProcessFile(String^ id, String^ dirID, String^ path, int depth, String^ crc) override;
	virtual void willProcessDirectory(String^ id, String^ path, int depth) override;

	void fileCreated(FileSystemEventArgs^ e);
	void fileDeleted(FileSystemEventArgs^ e);
	void fileRenamed(RenamedEventArgs^ e);

	void processQueue();
	void writeToFile();
private:
	property Hashtable^ directoryList; // using hashtable for O(1) specific search and inserting
	property Queue^ processingQueue; // queue of files/directories added to process 
	//TODO: could change to set in future
	property Boolean directoryModified; // event flag is need to write to disk
	//TODO: combine with get directoryList

	void handleDirectoryRename(String^ path, String^ newBasePath, String^ oldBasePath);
};

