#pragma once
#include "stdafx.h"
#define kVersion "0.9"

#define kIDFileName "~listOfFilesInCRC.txt"
#define kDirLinePrefix "~"
#define kLeftoverFolderName "~leftovers"

#define kKiloByte 1024
#define kMegaByte 1048576
#define kBufferSize 20 * kMegaByte

#define kSecond 1000
#define kMinute 60 * kSecond
#define kScheduledTimer 1 * kMinute

#define kCRC32Length 8

#define kLocationsFile "SynchiveMonitor_Locations.ini"


#define kStoragePath (Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "\\Synchive\\")
#define kFileNamePrefix "SynchiveMonitor_"
#define kFileNameVersionPrefix (kFileNamePrefix + "v")
#define kFileNameExtension ".exe"
#define kFileName (kFileNameVersionPrefix + kVersion + kFileNameExtension)


#define kSpecialKeyword "-monitor"
#define kStartAllKeyword "-startAll"


#define kEventSchedulerFolder "Synchive"
#define kEventSchTskPrefix (kEventSchedulerFolder + "\\" + kFileNameVersionPrefix)
#define kEventSchTskName (kEventSchTskPrefix + kVersion)

