#pragma once
#include <Windows.h>

struct BootSector {
	UINT16 bytesPerSector;
	UCHAR sectorsPerCluster;
	UINT16 reservedAreaSize;
	UCHAR numberOfFATs;
	UINT16 numberOfSectors2;
	UINT32 numberOfSectors4;
	UINT32 sizeOfFat;
	UINT16 numberOfUpdateFat;
	UINT32 rootDirectoryCluster;
};