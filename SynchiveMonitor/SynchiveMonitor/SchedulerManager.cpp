#include "stdafx.h"
#include "SchedulerManager.h"


SchedulerManager::SchedulerManager()
{
	wchar_t buffer[1024];
	GetModuleFileName(NULL, buffer, 1024);
	
	processPath = gcnew String(buffer);
	Console::WriteLine(processPath);
}

String^ SchedulerManager::executeCommand(SchedulerManager::Query type, String^ path)
{
	if (path != nullptr)
	{
		path = path->Replace("\"", ""); // stripe out quoted path
	}
	if (path != nullptr && !Directory::Exists(path))
	{
		return "Invliad Directory!";
	}

	String^ arguments = getQueryForType(type, path); // get arguments for task scheduler
	
	if (type == SchedulerManager::Query::oneTime)
	{
		Process^ p = gcnew Process();
		ProcessStartInfo^ ps = gcnew ProcessStartInfo(kStoragePath + kFileName , "\"" + path + "\"");
		//ps->WindowStyle = Diagnostics::ProcessWindowStyle::Hidden;
		p->StartInfo = ps;
		p->Start();

		return "Running One Time!";
	}
	else
	{
		Process^ p = gcnew Process();
		ProcessStartInfo^ ps = gcnew ProcessStartInfo("schtasks", arguments);
		ps->RedirectStandardOutput = true;
		ps->UseShellExecute = false;
		//ps->WindowStyle = Diagnostics::ProcessWindowStyle::Hidden;
		p->StartInfo = ps;
		p->Start();

		StreamReader^ sc = p->StandardOutput;
		return sc->ReadToEnd();
	}
	
}

String^ SchedulerManager::getQueryForType(SchedulerManager::Query type, String^ path)
{
	Directory::GetCurrentDirectory();
	String^ arguments = "";
	String^ convertedPath = path != nullptr ? (kEventSchedulerBase + "\\" + convertPathToInteral(path)) : "";
	Random^ rando = gcnew Random();
	switch (type)
	{
	case SchedulerManager::Query::newLocation:
		arguments = "/create /tn " + convertedPath + " /tr \"" + (kStoragePath + kFileName + " " + kSpecialKeyword + " " + path) + "\" /sc onlogon /f /delay 000" + rando->Next(1) + ":" + rando->Next(10,59);
		
		break;
	case SchedulerManager::Query::oneTime:
		//arguments = "/query /fo list";
		break;
	case SchedulerManager::Query::remove:
		arguments = "/delete /tn " + convertedPath + " /f";
		break;
	case SchedulerManager::Query::list:
		arguments = "/query /fo list";
		break;
	default:
		break;
	}
	Console::WriteLine("arguments:" + arguments);
	delete rando;
	return arguments;
}

/*
Folder: 

HostName: XXX
TaskName: Synchive\FolderName
Next Run Time:
Status:
Logon Mode:
*/
ArrayList^ SchedulerManager::parseOutput(String^ str)
{
	ArrayList^ output = gcnew ArrayList();
	StringReader^ r = gcnew StringReader(str);
	String^ line;
	array<wchar_t>^ filter = gcnew array<wchar_t>(1); // required otherwise split returns more than max substrings
	filter[0] = ' ';
	while ((line = r->ReadLine()) != nullptr)
	{
		//Console::WriteLine(line);
		if (line->Length == 0)
		{
			continue;
		}
		
		array<String^>^ splitStr = line->Split(filter , 2);
		
		// since system lists by folder, scan per folder
		if (splitStr[0] == "Folder:" && splitStr[1]->Contains(kEventSchedulerBase))
		{
			// while within folder read each line
			while ((line = r->ReadLine()) != nullptr && !line->StartsWith("Folder"))
			{
				//Console::WriteLine(line);

				splitStr = line->Split(filter, 2, StringSplitOptions::RemoveEmptyEntries);
				if (splitStr->Length > 0 && splitStr[0] == "TaskName:")
				{
					String^ base = kEventSchedulerBase;
					String^ trimmed = splitStr[1]->Substring(base->Length + 2); // 2 for '\'Synchive'\'Path
					output->Add(convertInteraltoPath(trimmed));
				}
			}
		}
	}
	delete filter;
	return output;
}

void SchedulerManager::validateMonitorFile()
{
	//Console::WriteLine(kStoragePath + kFileName);
	// centralize pl0x
	if (File::Exists(kStoragePath + kFileName)) // remove older version (centralize version upgrade)
	{
		File::Delete(kStoragePath + kFileName);
	}
	Directory::CreateDirectory(kStoragePath);
	File::Copy(processPath, kStoragePath + kFileName); // will not override existing file
}

String ^ SchedulerManager::newLocation(String ^ path)
{
	validateMonitorFile();
	String^ output = executeCommand(SchedulerManager::Query::newLocation, path);
	return output;
}

String ^ SchedulerManager::oneTime(String ^ path)
{
	validateMonitorFile();
	String^ output = executeCommand(SchedulerManager::Query::oneTime, path);
	return output;
}

String ^ SchedulerManager::removeLocation(String ^ path)
{
	String^ output = executeCommand(SchedulerManager::Query::remove, path);
	return output->Replace(kSlashReplacement, "\\");  // removes internal phrase when system spits task name back
}

String ^ SchedulerManager::removeAll()
{
	String^ output = "";
	String^ toRemove = executeCommand(SchedulerManager::Query::list, nullptr);
	ArrayList^ outputList = parseOutput(toRemove);

	for each(String^ path in outputList)
	{
		//Console::WriteLine(path);
		output += executeCommand(SchedulerManager::Query::remove, path) + "\n";
	}

	return output->Replace(kSlashReplacement, "\\"); // removes internal phrase when system spits task name back
}

String ^ SchedulerManager::listLocations()
{
	String^ finalOutput = "";
	String^ output = executeCommand(SchedulerManager::Query::list, nullptr);

	ArrayList^ outputList = parseOutput(output);
	for each(String^ str in outputList)
	{
		finalOutput += str + "\n";
	}
	return finalOutput;
}


String ^ SchedulerManager::convertPathToInteral(String^ path)
{
	String^ converted = path->Replace("\\", kSlashReplacement);
	converted = converted->Replace("\/", kSlashReplacement);
	converted = converted->Replace("\:", kColonReplacement);
	converted = converted->Replace(" ", kSpaceReplacement);
	//Console::WriteLine("@convertPathToInteral: " + converted);
	return converted;
}

String ^ SchedulerManager::convertInteraltoPath(String^ path)
{
	String^ converted = path->Replace(kSlashReplacement, "\\");
	converted = converted->Replace(kColonReplacement, "\:");
	converted = converted->Replace(kSpaceReplacement, " ");
	//Console::WriteLine("@convertInteraltoPath: " + converted );
	return converted;
}
