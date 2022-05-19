#pragma once
#include "DiskInfo.h"
class Reader
{
	HANDLE diskHandle;
public:
	bool openDevice(wstring diskName);
	UINT64 readSector(UINT64 initialSector, UINT64 sectorSize, UINT64 bytesToRead, UCHAR* buffer);
};

