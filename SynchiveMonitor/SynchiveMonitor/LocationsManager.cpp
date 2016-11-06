#include "stdafx.h"
#include "LocationsManager.h"


LocationsManager::LocationsManager()
{
	wchar_t buffer[1024];
	GetModuleFileName(NULL, buffer, 1024);

	processPath = gcnew String(buffer);
	//Console::WriteLine(processPath);
	locationList = gcnew ArrayList();
	readInLocations();

	validateSynchiveMonitorFile();
	validateSynchiveMonitorLocations();
}

LocationsManager::~LocationsManager()
{
	delete locationList;
	delete processPath;
}

void LocationsManager::readInLocations()
{
	StreamReader sr(kStoragePath + kLocationsFile);
	String^ str;

	while((str = sr.ReadLine()) != nullptr)
	{
		locationList->Add(str->ToLower());
	}
	sr.Close();
}

void LocationsManager::startMonitoringLocations()
{
	for(int i = 0; i < locationList->Count; i++)
	{
		runWithLatest((String^)locationList[i]);
	}
}

// creates a new task in task scheduler that will run on login, isPersistent = true = run on logon
String^ LocationsManager::newLocation(String^ path, bool isPersistent)
{
	if(!Directory::Exists(path))
	{
		return "Bad Path";
	}
	path = normalizeSlashes(path);

	if(listContains(path->ToLower()))
	{
		return "Path already monitored";
	};

	if(isPersistent) // if location not already monitored, write to file and monitor
	{
		StreamWriter sw(kStoragePath + kLocationsFile, true);
		sw.WriteLineAsync(path);
		sw.Close();

		locationList->Add(path);
	}
	
	runWithLatest(path);
	return "Monitoring " + path;
}

// removes a specific location from task scheduler
String^ LocationsManager::removeLocation(String^ path)
{
	path = normalizeSlashes(path)->ToLower();

	if(!listContains(path->ToLower()))
	{
		return "Location not found";
	}

	String^ locs = "";
	StreamWriter sw(kStoragePath + kLocationsFile);
	String^ str;

	for(int i = 0; i < locationList->Count; i++)
	{
		sw.WriteLineAsync((String^)locationList[i]);
	}
	sw.Close();
	return path;
}

// all scheduled task
void LocationsManager::removeAll()
{
	delete locationList;
	locationList = gcnew ArrayList;
	FileStream^ stream = File::Create(kStoragePath + kLocationsFile);
	stream->Close();
}

// list all scheduled task currently being monitored
String ^ LocationsManager::listLocations()
{
	String^ finalOutput = "";

	for(int i = 0; i < locationList->Count; i++)
	{
		finalOutput += locationList[i] + "\n";
	}
	return finalOutput;
}

void LocationsManager::runWithLatest(String^ path)
{
	path = normalizeSlashes(path);
	DirectoryInfo dirInfo(kStoragePath);
	String^ latestVersion = kVersion;
	array<String^>^ filter = {"_v"}; // required otherwise split returns more than max substrings
	String^ extension = kFileNameExtension;

	for each(FileInfo^ fInfo in dirInfo.EnumerateFiles(kFileNamePrefix + "*"))
	{
		if(!fInfo->Name->StartsWith(kFileNameVersionPrefix))
		{
			continue;
		}
		array<String^>^ splitStr = fInfo->Name->Split(filter, StringSplitOptions::RemoveEmptyEntries);
		String^ version = splitStr[1]->Substring(0, splitStr[1]->Length - extension->Length);
		if(Double::Parse(version) > Double::Parse(latestVersion))
		{
			latestVersion = version;
		}
	}
	delete filter;

	String^ newMonitorPath = kStoragePath + kFileNamePrefix + "v" + latestVersion + kFileNameExtension;

	Process^ p = gcnew Process();
	ProcessStartInfo^ ps = gcnew ProcessStartInfo(newMonitorPath, kSpecialKeyword + " \"" + path + "\"");
	//ps->WindowStyle = Diagnostics::ProcessWindowStyle::Hidden;
	p->StartInfo = ps;
	p->Start();
	//TODO check if required to delete or create on stack
}

// ensure SynchiveMonitor.exe exist in storage
void LocationsManager::validateSynchiveMonitorFile()
{
	try
	{
		Directory::CreateDirectory(kStoragePath);
		File::Copy(processPath, kStoragePath + kFileName); // will not override existing file
	}
	catch(UnauthorizedAccessException^ e) // file in use
	{

	}
	catch(IOException^ e)
	{

	}
	// TODO: check for latest version and use that one
	//maybe delete old versions too?
}


// ensure SynchiveMonitor.exe exist in storage
void LocationsManager::validateSynchiveMonitorLocations()
{
	try
	{
		if(!File::Exists(kStoragePath + kLocationsFile))
		{
			File::Create(kStoragePath + kLocationsFile);
		}
	}
	catch(UnauthorizedAccessException^ e) // file in use
	{

	}
	catch(IOException^ e)
	{

	}
	// TODO: check for latest version and use that one
	//maybe delete old versions too?
}

// gets a unique taskname to reference
String^ LocationsManager::normalizeSlashes(String^ path)
{
	String^ converted = path->Replace("/", "\\");
	return converted;
}

bool LocationsManager::listContains(String^ path)
{
	for(int i = 0; i < locationList->Count; i++)
	{
		if((String^)locationList[i] == path)
		{
			return true;
		}
	}
	return false;
}