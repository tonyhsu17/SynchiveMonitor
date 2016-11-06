#pragma once
/*
Handles starting up monitoring locations through XXX.txt.



*/
ref class LocationsManager
{
public:
	LocationsManager();
	~LocationsManager();
	String^ startMonitoringLocations();

	String ^ newLocation(String ^ path);

	String ^ removeLocation(String ^ path);

	void removeAll();

	String ^ listLocations();
		

	void validateSynchiveMonitorFile();
	void validateSynchiveMonitorLocations();
	String ^ normalizeSlashes(String ^ path);
	void startProcess(String ^ path);
	

private:
	property String^ processPath; // path of the current process (aka self)

	
	void runWithLatest(String ^ path);
};

