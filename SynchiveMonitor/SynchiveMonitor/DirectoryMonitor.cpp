#include "stdafx.h"
#include "DirectoryMonitor.h"


DirectoryMonitor::DirectoryMonitor(String^ path)
{
	runWithLatest(path);

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

void DirectoryMonitor::runWithLatest(String^ path)
{
	DirectoryInfo^ dirInfo = gcnew DirectoryInfo(kStoragePath);
	String^ latestVersion = kVersion;
	array<String^>^ filter = gcnew array<String^>(1); // required otherwise split returns more than max substrings
	filter[0] = "_v";
	String^ extension = kFileNameExtension;

	for each(FileInfo^ fInfo in dirInfo->EnumerateFiles(kFileNamePrefix + "*"))
	{
		array<String^>^ splitStr = fInfo->Name->Split(filter, StringSplitOptions::RemoveEmptyEntries);
		String^ version = splitStr[1]->Substring(0, splitStr[1]->Length - extension->Length);
		if(Double::Parse(version) > Double::Parse(latestVersion))
		{
			latestVersion = version;
		}
	}
	delete filter;

	// if newer version found, start with newer version
	if(Double::Parse(latestVersion) > Double::Parse(kVersion))
	{
		SchedulerManager^ manager = gcnew SchedulerManager();
		manager->removeLocation(path);
		manager->newLocation(path, latestVersion);

		String^ newMonitorPath = kStoragePath + kFileNamePrefix + "v" + latestVersion + kFileNameExtension;

		Process^ p = gcnew Process();
		ProcessStartInfo^ ps = gcnew ProcessStartInfo(newMonitorPath, kSpecialKeyword + " \"" + path + "\"");
		//ps->WindowStyle = Diagnostics::ProcessWindowStyle::Hidden;
		p->StartInfo = ps;
		p->Start();

		exit(0);
	}
}