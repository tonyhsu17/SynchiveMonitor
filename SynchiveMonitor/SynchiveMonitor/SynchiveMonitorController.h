#pragma once
#include "Terminal.h"
#include "LocationsManager.h"
#include "DirectoryMonitor.h"
#include "WinTaskScheduler.h"

ref class SynchiveMonitorController
{
public:
	SynchiveMonitorController(array<String ^> ^args);
	~SynchiveMonitorController();
	void startTerminal();
	void run();

private:
	array<String^> ^args;
	Terminal^ terminal;
	TerminalDelegate^ delegator;
	DirectoryMonitor^ dirMon;
	LocationsManager^ locManager;

	void handleTerminalDelegate(TerminalDelegateType type, String^ location);
};

