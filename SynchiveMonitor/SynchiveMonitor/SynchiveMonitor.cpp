// SynchiveMonitor.cpp : main project file.
#include "stdafx.h"
#include "SynchiveMonitorController.h"

using namespace System;

// arguments:
// -new			creates a new on-startup task to monitor location
// -delete		remove monitoring of specific location
// -remove		remove monitoring of specific location
// "location"	directory to monitor
// -removeall	removes all scheduled tasks
// -deleteall	removes all scheduled tasks
// -list		lists all monitoring locations
// TODO allow smart removals (contains keyword) instead of full path
// TODO allow diff writting time
// TODO allow killing specific monitoring process
int main(array<System::String ^> ^args)
{
	//DirectoryMonitor^ mon = gcnew DirectoryMonitor("D:\\Synchive Monitor Test Folder");
	//mon->run();

	SynchiveMonitorController^ controller = gcnew SynchiveMonitorController(args);
	controller->run();
	delete controller;
	return 0;
}




