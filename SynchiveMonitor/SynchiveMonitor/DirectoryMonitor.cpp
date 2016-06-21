#include "stdafx.h"
#include "DirectoryMonitor.h"


DirectoryMonitor::DirectoryMonitor(String^ path)
{
	manager = gcnew DirectoryManagement(path);
	manager->readinIDs();

	monitor = gcnew FileSystemWatcher;
	monitor->Path = path;
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
	monitor->Created += gcnew FileSystemEventHandler(DirectoryMonitor::fileCreated);
	monitor->Deleted += gcnew FileSystemEventHandler(DirectoryMonitor::fileDeleted);
	monitor->Renamed += gcnew RenamedEventHandler(DirectoryMonitor::fileRenamed);

	// Add processing timer
	timer = gcnew Timer(kScheduledTimer);
	timer->BeginInit();
	timer->AutoReset = true;
	timer->Elapsed += gcnew ElapsedEventHandler(handleTimer);
	timer->EndInit();
}

int DirectoryMonitor::run()
{
	monitor->EnableRaisingEvents = true;
	timer->Start();

	while (true)
	{
		System::Threading::Thread::Sleep(60 * kMinute);
	}
	return 0;
}

void DirectoryMonitor::fileCreated(Object ^ sender, FileSystemEventArgs ^ e)
{
	manager->fileCreated(e);
}

void DirectoryMonitor::fileDeleted(Object ^ sender, FileSystemEventArgs ^ e)
{
	manager->fileDeleted(e);
}

void DirectoryMonitor::fileRenamed(Object ^ sender, RenamedEventArgs ^ e)
{
	manager->fileRenamed(e);
}

void DirectoryMonitor::handleTimer(Object^ sender, ElapsedEventArgs^ e)
{
	Console::WriteLine("Timer event");
	manager->processQueue();
	manager->writeToFile();
}