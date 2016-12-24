#pragma once
/*
Acts as a view controller
*/

enum class TerminalDelegateType {
	MonitorNewLocation, MonitorLocationOnce, ListLocations, RemoveLocation, RemoveAllLocations,
};

public delegate void TerminalDelegate(TerminalDelegateType type, String^ location);

ref class Terminal
{
public:
	Terminal(TerminalDelegate^ delegatee);
	void runTerminal();
	void writeLine(String ^ str); // prints out to screen
	
private:
	TerminalDelegate^ delegatee;

	void sendToDelegate(TerminalDelegateType type, String^ location); // checks for null delegate before proceeding
	void printHelp();
};


