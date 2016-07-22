#pragma once
/*
Handles interfacing with "Task Scheduler" by using command line.
Also handles starting single instance monitoring.

Executable for monitoring is stored in %APPDATA%/Synchive
*/
ref class SchedulerManager
{
public:
	enum class Query {newLocation, oneTime, remove, list};
		
	SchedulerManager();

	String^ newLocation(String^ path); // creates a new task in task scheduler that will run on login
	String^ newLocation(String^ path, String^ version); // creates a new task in task scheduler with new version
	String^ oneTime(String^ path); // starts a single instance
	String^ removeLocation(String^ path); // removes a specific location from task scheduler
	String^ removeAll(); // all scheduled task
	String^ listLocations(); // list all scheduled task currently being monitored
	
	
private:
	property String^ processPath; // path of the current process (aka self)

	String^ executeCommand(SchedulerManager::Query type, String ^ path); // starts the task through commandline
	String^ executeCommand(SchedulerManager::Query type, String ^ path, String^ version); // starts the task through commandline

	String^ getArgsForType(SchedulerManager::Query type, String^ path, String^ version); // get the arguments for schtasks
	
	ArrayList^ parseOutput(String^ str); // get important information out of schtask output

	void validateMonitorFile(); // ensure SynchiveMonitor.exe exist in storage

	String^ convertPathToInteral(String^ path); // gets a unique taskname to reference
	String^ convertInteraltoPath(String^ path); // gets the path name from taskname
	

};

