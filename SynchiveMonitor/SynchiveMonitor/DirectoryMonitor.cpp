#include "stdafx.h"
#include "DirectoryMonitor.h"


DirectoryMonitor::DirectoryMonitor()
{
	monitor = gcnew FileSystemWatcher;
	monitor->Path = "D:\\TestA";
	monitor->IncludeSubdirectories = true;
	monitor->Filter = "";
	/* Watch for changes in LastAccess and LastWrite times, and
	the renaming of files or directories. */
	monitor->NotifyFilter = static_cast<NotifyFilters>(
		NotifyFilters::LastAccess |
		NotifyFilters::LastWrite |
		NotifyFilters::FileName |
		NotifyFilters::DirectoryName);
	monitor->EnableRaisingEvents = false;

	// Add event handlers
	//monitor->Changed += gcnew FileSystemEventHandler(DirectoryMonitor::fileChanged);
	monitor->Created += gcnew FileSystemEventHandler(DirectoryProcessor::fileCreated);
	monitor->Deleted += gcnew FileSystemEventHandler(DirectoryProcessor::fileDeleted);
	monitor->Renamed += gcnew RenamedEventHandler(DirectoryProcessor::fileRenamed);
}

int DirectoryMonitor::run()
{
	monitor->EnableRaisingEvents = true;
	while (true);
	return 0;
}