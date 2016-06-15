#pragma once

#include "stdafx.h"
#include "Utilities.h"
#include "DirectoryProcessor.h"

ref class DirectoryMonitor
{
public:
	DirectoryMonitor();
	int run();
private:
	FileSystemWatcher^ monitor;
};

