#pragma once

#include "stdafx.h"
#include "Globals.h"
#include "DirectoryProcessor.h"

ref class DirectoryMonitor
{
public:
	DirectoryMonitor();
	int run();

private:
	property FileSystemWatcher^ monitor;
};

