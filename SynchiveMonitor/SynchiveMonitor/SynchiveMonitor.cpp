// SynchiveMonitor.cpp : main project file.
#include "stdafx.h"
#include "DirectoryMonitor.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	Console::WriteLine(L"Hello World");

	DirectoryProcessor^ prc = gcnew DirectoryProcessor("D:\\TestB");
	prc->readinIDs();
	DirectoryMonitor^ mon = gcnew DirectoryMonitor();
	mon->run();
	return 0;
}
