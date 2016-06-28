#pragma once
#include "stdafx.h"
#define kVersion "v0.1"

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

#define kEventSchedulerBase "Synchive"
#define kSlashReplacement "%@%@%"
#define kColonReplacement "!#!#!"
#define kSpaceReplacement "@#@#@"

#define kStoragePath (Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "\\Synchive\\")
#define kFileName "SynchiveMonitor_v0.1.exe"

#define kSpecialKeyword "-monitor"
