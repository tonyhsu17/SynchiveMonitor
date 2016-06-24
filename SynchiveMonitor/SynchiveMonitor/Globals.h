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

#define kEventSchedulerBase "\\Synchive"
#define kSlashReplacement "%@%@%"
#define kStorageLocation "%appdata%\\Synchive\\"

#define kNewFlagMask			0x01 // 0000 0001
#define kRemoveFlagMask			0x02 // 0000 0010
#define kRemoveAllFlagMask		0x04 // 0000 0100
#define kListSpecificFlagMask	0x08 // 0000 1000
#define kListAllFlagMask		0x10 // 0001 0000