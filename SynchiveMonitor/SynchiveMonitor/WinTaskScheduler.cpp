#include "stdafx.h"
#include "WinTaskScheduler.h"


String^ WinTaskScheduler::createOnLogonTask()
{
	if(checkVersion() > 0)
	{
		return "";
	}
	
	String^ filePath = kStoragePath + kFileName;
	String^ args = "/create /tn " + kEventSchTskName + " /tr \"" + (filePath + " " + kStartAllKeyword) + "\" /sc onlogon /f";
	return executeTaskSchCommandLine(args);
}

String^ WinTaskScheduler::executeTaskSchCommandLine(String^ args)
{
	Process^ p = gcnew Process();
	ProcessStartInfo^ ps = gcnew ProcessStartInfo("schtasks", args);
	ps->RedirectStandardOutput = true;
	ps->UseShellExecute = false;
	p->StartInfo = ps;
	p->Start();

	StreamReader^ sc = p->StandardOutput;
	return sc->ReadToEnd();
}

// get important information out of schtask output
/*
Folder:
HostName: XXX
TaskName: Synchive\FolderName
Next Run Time:
Status:
Logon Mode:
*/
Double WinTaskScheduler::checkVersion()
{
	String^ args = "/query /fo list";
	String^ str = executeTaskSchCommandLine(args);

	StringReader^ r = gcnew StringReader(str);
	String^ line;
	array<wchar_t>^ filter = {' '};

	while((line = r->ReadLine()) != nullptr)
	{
		if(line->Length == 0)
		{
			continue; // skip empty lines
		}

		array<String^>^ splitStr = line->Split(filter, 2);

		// since system lists by folder, scan per folder
		if(splitStr[0] == "Folder:" && splitStr[1]->Contains(kEventSchedulerFolder))
		{
			// while within folder read each line
			while((line = r->ReadLine()) != nullptr && !line->StartsWith("Folder"))
			{
				splitStr = line->Split(filter, 2, StringSplitOptions::RemoveEmptyEntries);
				if(splitStr->Length > 0 && splitStr[0] == "TaskName:")
				{
					array<String^>^ versionFilter = {"_v"};

					array<String^>^ trimmed = splitStr[1]->Split(versionFilter, StringSplitOptions::RemoveEmptyEntries);
					return Double::Parse(trimmed[1]);
				}
			}
		}
	}
	return 0; // no task found
}

// Manually update task scheduler so other versions can be deleted
// Will always try to find latest version to run anyways
String ^ WinTaskScheduler::updateVersion()
{
	throw gcnew System::NotImplementedException();
	// TODO: insert return statement here
}
