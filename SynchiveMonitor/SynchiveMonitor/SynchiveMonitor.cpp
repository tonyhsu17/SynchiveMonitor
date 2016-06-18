// SynchiveMonitor.cpp : main project file.
#include "stdafx.h"
#include "DirectoryMonitor.h"

#include "CRC32.h"
using namespace System;

int main(array<System::String ^> ^args)
{
	Console::WriteLine(L"Hello World");
	/*DirectoryProcessor^ prc = gcnew DirectoryProcessor("D:\\TestB");
	prc->readinIDs();
	DirectoryMonitor^ mon = gcnew DirectoryMonitor();
	mon->run();*/

	CRC32^ crc = gcnew CRC32();
	
	String^ fileName = "D:\\TestB\\hi\\test[352441C2].mkv";
	String^ temp = crc->computeHash(fileName);
	Console::WriteLine(temp);
	return 0;
}
