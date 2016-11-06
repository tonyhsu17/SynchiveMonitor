#include "stdafx.h"
#include "Terminal.h"
//#include <taskschd.h>

Terminal::Terminal(TerminalDelegate ^ delegatee)
{
	this->delegatee = delegatee;
}


void Terminal::runTerminal()
{
	String^ str;
	array<String^>^ splitStr;
	array<wchar_t>^ filter = {' '};

	Console::WriteLine("Welcome to Synchive Monitor " + kVersion);
	printHelp();
	Console::Write(">");

	while((str = Console::ReadLine()->ToLower()) != "q")
	{
		if(str->ToLower()->StartsWith("new "))
		{
			splitStr = str->Split(filter, 2);
			sendToDelegate(TerminalDelegateType::MonitorNewLocation, splitStr[1]);
		}
		else if(str->ToLower()->StartsWith("once "))
		{
			splitStr = str->Split(filter, 2);
			sendToDelegate(TerminalDelegateType::MonitorLocationOnce, splitStr[1]);
		}
		else if(str->ToLower()->StartsWith("list"))
		{
			sendToDelegate(TerminalDelegateType::ListLocations, nullptr);
		}
		else if(str->ToLower()->StartsWith("removeall"))
		{
			sendToDelegate(TerminalDelegateType::RemoveAllLocations, nullptr);
		}
		else if(str->ToLower()->StartsWith("remove "))
		{
			splitStr = str->Split(filter, 2);
			sendToDelegate(TerminalDelegateType::RemoveLocation, splitStr[1]);
		}
		else if(str->ToLower()->StartsWith("help") || 
			str->StartsWith("/?") ||
			str->StartsWith("\\?"))
		{
			printHelp();
		}
		else
		{
			Console::WriteLine("Invalid Command");
		}
		Console::Write(">");
	}
}

void Terminal::writeLine(String^ str)
{
	if(str->Length > 0)
	{
		Console::WriteLine(str);
	}
}

void Terminal::sendToDelegate(TerminalDelegateType type, String^ location)
{
	if(delegatee == nullptr)
	{
		return;
	}
	delegatee(type, location);
}

void Terminal::printHelp()
{
	Console::WriteLine("Schedule new monitoring, usage: \"new <path>\"");
	Console::WriteLine("Run single instance, usage: \"once <path>\"");
	Console::WriteLine("List monitoring locations, usage: \"list\"");
	Console::WriteLine("Remove monitoring location, usage: \"remove <path>\"");
	Console::WriteLine("Remove all monitoring locs, usage: \"removeall\"");
}