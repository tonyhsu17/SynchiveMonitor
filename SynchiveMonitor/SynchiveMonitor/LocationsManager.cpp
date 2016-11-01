#include "stdafx.h"
#include "LocationsManager.h"


LocationsManager::LocationsManager()
{
	wchar_t buffer[1024];
	GetModuleFileName(NULL, buffer, 1024);

	processPath = gcnew String(buffer);
	Console::WriteLine(processPath);
	validateSynchiveMonitorFile();
	validateSynchiveMonitorLocations();
}

String^ LocationsManager::startMonitoringLocations()
{
	if(!File::Exists(kStoragePath + kLocationsFile))
	{
		return "No directories found within file";
	}

	String^ output = "";
	
	StreamReader^ sc = gcnew StreamReader(kStoragePath + kLocationsFile);
	String^ str;
	while((str = sc->ReadLine()) != nullptr)
	{
		//start monitoring str
		startProcess(str);
		output += "Monitoring: " + str + "\n";
	}
	sc->Close();
}

// creates a new task in task scheduler that will run on login
String^ LocationsManager::newLocation(String^ path)
{
	path = normalizeSlashes(path);
	if(!Directory::Exists(path))
	{
		return "Bad Path";
	}
	StreamReader^ sc = gcnew StreamReader(kStoragePath + kLocationsFile);
	String^ str;
	while((str = sc->ReadLine()) != nullptr)
	{
		if(str == path)
		{
			sc->Close();
			return "Path already monitored";
		}
	}
	sc->Close();
	
	// if location not already monitoed, write to file and monitor
	StreamWriter^ sw = gcnew StreamWriter(kStoragePath + kLocationsFile, true);
	sw->WriteLine(path);
	startProcess(path);
	sw->Close();
	return "Monitoring " + path;
}

// removes a specific location from task scheduler
String^ LocationsManager::removeLocation(String^ path)
{
	path = normalizeSlashes(path);
	String^ locs = "";
	StreamReader^ sc = gcnew StreamReader(kStoragePath + kLocationsFile);
	String^ str;
	while((str = sc->ReadLine()) != nullptr)
	{
		if(str == path)
		{
			// delete location
			// check with absolute path and check final path name for match (if single match delete that one)
			// return deleted path
			continue;
		}
		locs += str + "\n";
	}
	sc->Close();

	//TODO finish, rewrite file 
	//StreamWriter^ sw = gcnew StreamWriter(kStoragePath + kLocationsFile);
	
	return path + " removed";
}

// all scheduled task
void LocationsManager::removeAll()
{
	// delete file
}

// list all scheduled task currently being monitored
String ^ LocationsManager::listLocations()
{
	String^ finalOutput = "";
	String^ locs = "";
	StreamReader^ sc = gcnew StreamReader(kStoragePath + kLocationsFile);
	String^ str;
	while((str = sc->ReadLine()) != nullptr)
	{
		finalOutput += str + "\n";
	}
	sc->Close();
	return finalOutput;
}


void LocationsManager::startProcess(String^ path)
{
	runWithLatest(path);
}

void LocationsManager::runWithLatest(String^ path)
{
	path = normalizeSlashes(path);
	DirectoryInfo^ dirInfo = gcnew DirectoryInfo(kStoragePath);
	String^ latestVersion = kVersion;
	array<String^>^ filter = gcnew array<String^>(1); // required otherwise split returns more than max substrings
	filter[0] = "_v";
	String^ extension = kFileNameExtension;

	for each(FileInfo^ fInfo in dirInfo->EnumerateFiles(kFileNamePrefix + "*"))
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
	ps->WindowStyle = Diagnostics::ProcessWindowStyle::Hidden;
	p->StartInfo = ps;
	p->Start();
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