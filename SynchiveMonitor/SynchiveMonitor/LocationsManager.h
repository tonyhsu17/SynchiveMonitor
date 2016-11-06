#pragma once
/*
Handles starting up monitoring locations through XXX.txt.



*/


ref class LocationsManager
{
public:
	LocationsManager();
	~LocationsManager();

	void startMonitoringLocations();
	void readInLocations();
	String^ newLocation(String ^ path, bool isPersistent);
	String^ removeLocation(String ^ path);
	String^ listLocations();
	void removeAll();

	String^ normalizeSlashes(String ^ path);
	
private:
	property String^ processPath; // path of the current process (aka self)
	ArrayList^ locationList; 

	void validateSynchiveMonitorFile();
	void validateSynchiveMonitorLocations();
	void runWithLatest(String^ path);
	bool listContains(String ^ path);
};

