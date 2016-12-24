#pragma once
/*
Class to handle starting up monitoring for each location specified in SynchiveMonitor_Locations.ini.
Manages adding or deleting monitoring locations

Author: Tony Hsu
*/


ref class LocationsManager
{
public:
	// ~~~~~ methods ~~~~~ //

	// default constructor that also makes sure SynchiveMonitor_Locations.ini file exist
	// and SynchiveMonitor.exe program is copied into working directory for on login purposes
	LocationsManager();  
	~LocationsManager();

	// start the monitoring of each location
	void startMonitoringLocations();
	// create a new location to monitor
	String^ newLocation(String^ path, bool isPersistent);
	// remove a location from monitoring
	String^ removeLocation(String^ path);
	// lists all locations being monitored
	String^ listLocations();
	// remove all locations being monitored
	void removeAll();

	
	
private:
	// ~~~~~ variables ~~~~~ //

	// path of the current process (aka self)
	property String^ processPath; 
	// list of monitoring locations
	ArrayList^ locationList; 

	// ~~~~~ methods ~~~~~ //

	// Parse and store paths from file
	void readInLocations();
	// ensure Program itself exist in working directory
	void validateSynchiveMonitorFile();
	// ensure monitoring paths file exsit in working directory
	void validateSynchiveMonitorLocations();
	// start new process with latest version
	void runWithLatest(String^ path);
	// check if path is being monitored
	bool listContains(String^ path);
	// converts forward slashes to backward slashes
	String^ normalizeSlashes(String^ path);
};

