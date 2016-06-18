#pragma once
#include "stdafx.h"

ref class FileProcessorBase abstract
{
public:
	value struct SynchiveFile {
		String^ path;
		int depth;
		String^ crc;
	};
	value struct SynchiveDirectory {
		String^ path;
		int depth;
	};

	FileProcessorBase(String^ path);
	void readinIDs();

	virtual void didProcessFile(String^ id, String^ path, int depth, String^ crc) abstract;
	virtual void willProcessDirectory(String^ id, String^ path, int depth) abstract;

	static String^ getDirectoryUniqueID(String^ filePath, int level, String^ rootPath);
	static String^ getFileUniqueID(String^ name, String^ crc);

private:
	property Hashtable^ directoyMapping;
	property StreamReader^ reader;
	property Stack^ directoriesToProcess;
	property SynchiveDirectory root;

	
	void readFilesWithinDirectory(SynchiveDirectory^ dir);
	void readFromIDFile(String^ path, int baseDepth);
	String^ calculateCRC32(String^ file);
};

