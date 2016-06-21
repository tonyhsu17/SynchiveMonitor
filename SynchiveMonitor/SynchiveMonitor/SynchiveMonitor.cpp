// SynchiveMonitor.cpp : main project file.
#include "stdafx.h"
#include "DirectoryMonitor.h"

#include "CRC32.h"
using namespace System;

int main(array<System::String ^> ^args)
{
	Console::WriteLine(L"Hello World");
	
	//DirectoryMonitor^ mon = gcnew DirectoryMonitor("E:\\Videos\\Anime");
	DirectoryMonitor^ mon = gcnew DirectoryMonitor("D:\\TestB");

	mon->run();


	return 0;
}
