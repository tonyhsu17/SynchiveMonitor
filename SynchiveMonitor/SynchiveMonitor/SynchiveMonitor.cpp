// SynchiveMonitor.cpp : main project file.
#include "stdafx.h"
#include "DirectoryMonitor.h"
#include "SchedulerManager.h"
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

	//DirectoryMonitor^ mon = gcnew DirectoryMonitor("E:\\Videos\\Anime");
	//DirectoryMonitor^ mon = gcnew DirectoryMonitor("D:\\TestB");

	//mon->run();
	
	//String^ output = system("schtasks /query /fo list");
	
	/*
	if args > 1
		if -nogui 
			that means create new process and monitor that directory
		else
			allow command line/input

	*/
	///*
	SchedulerManager^ manager = gcnew SchedulerManager();
	
	if (args->Length == 0)
	{
		//StreamReader^ sc = gcnew StreamReader();
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

		while ((str = Console::ReadLine()) != "q")
		{
			if (str->ToLower()->StartsWith("new"))
			{
				splitStr = str->Split(' ', 2);
				output = manager->newLocation(splitStr[1]);
			}
			else if (str->ToLower()->StartsWith("onetime"))
			{
				splitStr = str->Split(' ', 2);
				output = manager->oneTime(splitStr[1]);
			}
			else if (str->ToLower()->StartsWith("list"))
			{
				output = manager->listLocations();
			}
			else if (str->ToLower()->StartsWith("removeall"))
			{
				output = manager->removeAll();
			}
			else if (str->ToLower()->StartsWith("remove"))
			{
				splitStr = str->Split(' ', 2);
				output = manager->removeLocation(splitStr[1]);
			}
			else
			{
				continue;
			}

			Console::WriteLine("OUTPUT\n" + output);
		}
	}
	else if (args->Length == 1) 
	{
		// lol this is terrible assumption
		DirectoryMonitor^ mon = gcnew DirectoryMonitor(args[0]);
		mon->run();
	}
	//*/
	/*Byte flags = 0;


	Process^ p = gcnew Process();
	ProcessStartInfo^ ps = gcnew ProcessStartInfo("schtasks", "/query /fo list");
	ps->RedirectStandardOutput = true;
	ps->UseShellExecute = false;
	p->StartInfo = ps;
	p->Start();
	StreamReader^ sc = p->StandardOutput;

	Console::WriteLine(sc->ReadToEnd());*/

	return 0;
}



