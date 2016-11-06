#pragma once

#include "stdafx.h"
#include "Globals.h"
#include "DirectoryManagement.h"

ref class DirectoryMonitor
{
public:
	DirectoryMonitor(String^ path);
	int run();

	static void fileCreated(Object^ sender, FileSystemEventArgs^ e);
	static void fileDeleted(Object^ sender, FileSystemEventArgs^ e);
	static void fileRenamed(Object^ sender, RenamedEventArgs^ e);
private:
	property FileSystemWatcher^ monitor;
	static property DirectoryManagement^ manager;
	
	
	property Timer^ timer;
	static void handleTimer(Object^ sender, ElapsedEventArgs^ e);


};

