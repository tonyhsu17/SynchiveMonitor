// SynchiveMonitor.cpp : main project file.
#include "stdafx.h"
#include "SynchiveMonitorController.h"




SynchiveMonitorController::SynchiveMonitorController(array<String ^> ^args)
{
	this->args = args;
}

SynchiveMonitorController::~SynchiveMonitorController()
{
	delete terminal;
	delete delegator;
	delete dirMon;
}

void SynchiveMonitorController::run()
{
	if(args->Length > 0 && args[0] == kSpecialKeyword) // monitor location
	{
		String^ path = args[1];
		for(int i = 2; i < args->Length; i++)
		{
			path += " " + args[i];
		}

		Console::WriteLine("Monitoring: " + path); // debug purposes as normally the window is hidden
		dirMon = gcnew DirectoryMonitor(path);
		dirMon->run();
	}
	else if(args->Length > 0 && args[0] == kStartAllKeyword) // start monitoring all locations
	{
		locManager = gcnew LocationsManager();
		locManager->startMonitoringLocations(); //creates new processes
		delete locManager;
		delete locManager;
		delete locManager;
		delete locManager;
	}
	else // show terminal
	{
		startTerminal();
	}
}

void SynchiveMonitorController::startTerminal()
{
	delegator = gcnew TerminalDelegate(this, &SynchiveMonitorController::handleTerminalDelegate);
	terminal = gcnew Terminal(delegator);
	terminal->runTerminal();
}

void SynchiveMonitorController::handleTerminalDelegate(TerminalDelegateType type, String ^ location)
{
	throw gcnew System::NotImplementedException();
}
