#pragma once
#include "stdafx.h"

ref class FileProcessorBase abstract
{
public:
	value struct SynchiveFile {
		String^ path;
		int depth;
	};
	value struct SynchiveDirectory {
		String^ path;
		int depth;
	};

	FileProcessorBase(String^ path);
	void readinIDs();

	virtual void didProcessFile(String^ filePath, int level, String^ crc) abstract;
	virtual void willProcessDirectory(SynchiveDirectory^ dir) abstract;

	static String^ getDirectoryUniqueID(String^ filePath, int level, String^ rootPath);
private:

	Hashtable^ directoyMapping;
	StreamReader^ reader;
	Stack^ directoriesToProcess;
	SynchiveDirectory root;

	
	void readFilesWithinDirectory(SynchiveDirectory^ dir);
	void readFromIDFile(String^ path, int baseDepth);
};

