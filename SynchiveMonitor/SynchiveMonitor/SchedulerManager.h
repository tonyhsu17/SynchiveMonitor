#pragma once
ref class SchedulerManager
{
public:
	enum class Query { newLocation, oneTime, remove, list };
	
	
	SchedulerManager();

	String ^ executeCommand(SchedulerManager::Query type, String ^ path);

	



	String^ newLocation(String^ path);
	String^ oneTime(String^ path);
	String^ removeLocation(String^ path);
	String^ removeAll();

	String^ listLocations();
private:
	String^ processPath;

	ArrayList ^ parseOutput(String ^ str);
	void validateMonitorFile();
	String ^ getQueryForType(SchedulerManager::Query type, String^ path);

	String^ convertPathToInteral(String^ path);
	String^ convertInteraltoPath(String^ path);
};

