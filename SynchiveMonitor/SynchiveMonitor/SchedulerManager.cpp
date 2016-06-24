#include "stdafx.h"
#include "SchedulerManager.h"


SchedulerManager::SchedulerManager()
{
	wchar_t buffer[1024];
	GetModuleFileName(NULL, buffer, 1024);
	
	processPath = gcnew String(buffer);
	Console::WriteLine(processPath);
/*
schtasks /create /tn "TEST" /tr "C:\fixStartMenu.ps1" /sc onstart
*/
}

String^ SchedulerManager::executeCommand(SchedulerManager::Query type, String^ path)
{
	if (path != nullptr && !Directory::Exists(path))
	{
		return "Invliad Directory!";
	}

	String^ arguments = getQueryForType(type, path); // get arguments for task scheduler
	
	if (type == SchedulerManager::Query::oneTime)
	{
		Process^ p = gcnew Process();
		ProcessStartInfo^ ps = gcnew ProcessStartInfo(kStoragePath + kFileName , path);
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
	
	switch (type)
	{
	case SchedulerManager::Query::newLocation:
		arguments = "/create /tn " + convertedPath + " /tr \"" + kStoragePath + kFileName + "\" /sc onstart";
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
	while ((line = r->ReadLine()) != nullptr)
	{
		//Console::WriteLine(line);
		if (line->Length == 0)
		{
			continue;
		}
		array<String^>^ splitStr = line->Split(' ', 2); // somehow its returning length > 2
		
		// since system lists by folder, scan per folder
		if (splitStr[0] == "Folder:" && splitStr[splitStr->Length - 1]->Contains(kEventSchedulerBase))
		{
			// while within folder read each line
			while ((line = r->ReadLine()) != nullptr && !line->StartsWith("Folder"))
			{
				//Console::WriteLine(line);

				splitStr = line->Split(' ', 2); // somehow its returning length > 2
				if (splitStr[0] == "TaskName:")
				{
					String^ base = kEventSchedulerBase;
					String^ trimmed = splitStr[splitStr->Length - 1]->Substring(base->Length + 2); // 2 for '\'Synchive'\'Path
					output->Add(convertInteraltoPath(trimmed));
				}
			}
		}
	}
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

	//Console::WriteLine("@convertPathToInteral: " + converted);
	return converted;
}

String ^ SchedulerManager::convertInteraltoPath(String^ path)
{
	String^ converted = path->Replace(kSlashReplacement, "\\");
	converted = converted->Replace(kColonReplacement, "\:");
	//Console::WriteLine("@convertInteraltoPath: " + converted );
	return converted;
}
