#pragma once
#include "stdafx.h"
#include "CRC32.h"

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
	bool doesRootIDFileExist();

	virtual void didProcessFile(String^ id, String^ dirID, String^ path, int depth, String^ crc) abstract;
	virtual void willProcessDirectory(String^ id, String^ path, int depth) abstract;

	static String^ getDirectoryUniqueID(String^ filePath, int level, String^ rootPath);
	static String^ getFileUniqueID(String^ name, String^ crc);
	static SynchiveDirectory^ getSynchiveDirectory(String^ id, String^ root);
	static SynchiveFile^ getSynchiveFile(String^ id, String^ parent);
	static String^ calculateCRC32(String^ file);
	static int getDepth(String^ path, String^ root, Boolean isFile);
	static String^ getRelativePath(String^ path, String^ basePath);
	
	
protected:
	property SynchiveDirectory^ root;
private:
	property StreamReader^ reader;
	property Stack^ directoriesToProcess;
	

	void readFilesWithinDirectory(SynchiveDirectory^ dir);
	void readFromIDFile(String^ path, int baseDepth);
	
};

