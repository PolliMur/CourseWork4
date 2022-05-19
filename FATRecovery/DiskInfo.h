#pragma once
#include "Disk.h"

class DiskInfo
{
	vector<Disk> disks;

public:
	DiskInfo() {};
	~DiskInfo() {};

	void searchDisks();
	bool thereIsFAT32();
	vector<Disk> getDisks() const;
};

