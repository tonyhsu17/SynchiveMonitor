// SynchiveMonitor.cpp : main project file.
#include "stdafx.h"
#include "DirectoryMonitor.h"
#include "LocationsManager.h"
#include "CRC32.h"

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
	Console::WriteLine(L"Hello World");
	Console::WriteLine("ArgsCount: " + args->Length);

	//DirectoryMonitor^ mon = gcnew DirectoryMonitor("D:\\Synchive Monitor Test Folder");
	//DirectoryMonitor^ mon = gcnew DirectoryMonitor("E:\\Videos\\Anime");

	//mon->run();
	
	/*
	if args > 1
		if -nogui 
			that means create new process and monitor that directory
		else
			allow command line/input
	*/
	///*
	LocationsManager^ manager = gcnew LocationsManager();
	
	if (args->Length == 0)
	{
		Console::WriteLine("Welcome to Synchive Monitor " + kVersion);
		Console::WriteLine("1 Schedule new monitoring, usage: \"new <path>\""); 
		Console::WriteLine("2 One time monitoring, usage: \"onetime <path>\"");
		Console::WriteLine("3 List monitoring locs, usage: \"list\"");
		Console::WriteLine("4 Remove monitoring of specific, usage: \"remove <path>\"");
		Console::WriteLine("5 Remove all monitoring locs, usage: \"removeall\"");
		Console::WriteLine("Input noaw!: ");

		String^ str;
		array<String^>^ splitStr;
		String^ output;

		array<wchar_t>^ filter = gcnew array<wchar_t>(1); // required otherwise split returns more than max substrings
		filter[0] = ' ';

		Console::Write("Input: ");
		while ((str = Console::ReadLine()) != "q")
		{
			if (str->ToLower()->StartsWith("new"))
			{
				splitStr = str->Split(filter, 2);
				output = manager->newLocation(splitStr[1]);
			}
			else if (str->ToLower()->StartsWith("onetime"))
			{
				splitStr = str->Split(filter, 2);
				output = "Monitoring " + splitStr[1];
				manager->startProcess(splitStr[1]);
			}
			else if (str->ToLower()->StartsWith("list"))
			{
				output = manager->listLocations();
			}
			else if (str->ToLower()->StartsWith("removeall"))
			{
				output = "";
				manager->removeAll();
			}
			else if (str->ToLower()->StartsWith("remove"))
			{
				splitStr = str->Split(filter, 2);
				output = manager->removeLocation(splitStr[1]);
			}
			else
			{
				continue;
			}

			Console::WriteLine("Output:\n" + output);
			Console::Write("\nInput: ");
		}
		delete filter;
	}
	else if (args->Length > 0 && args[0] == kSpecialKeyword)
	{ 
		String^ path = args[1];
		for (int i = 2; i < args->Length; i++)
		{
			path += " " + args[i];
		}
		
		Console::WriteLine("Monitoring: " + path);
		DirectoryMonitor^ mon = gcnew DirectoryMonitor(path);
		mon->run();
	}

	return 0;
}




