#pragma once
ref class WinTaskScheduler
{
public:
	static String^ createOnLogonTask();
	static String^ executeTaskSchCommandLine(String ^ args);
	static Double checkVersion();
	static String^ updateVersion();
};

