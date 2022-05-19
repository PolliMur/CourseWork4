#include "DiskInfo.h"

void DiskInfo::searchDisks() {
	WCHAR diskName[MAX_PATH + 1];
	DWORD logicalDrives = GetLogicalDrives();
	WCHAR diskLabel[] = L"c:\\";
	for (int i = 0; i < 26; i++) {
		if ((logicalDrives >> i) & 1)
		{
			diskLabel[0] = 'A' + i;
			GetVolumeNameForVolumeMountPointW(diskLabel, diskName, MAX_PATH + 1);
			Disk disk = Disk(diskName, diskLabel);
			this->disks.push_back(disk);
		}
	}
}

bool DiskInfo::thereIsFAT32() {
	wstring fat = L"FAT32";
	bool flag = false;
	vector<Disk>::iterator it = disks.begin();

	for (it = disks.begin(); it != disks.end(); ++it) {
		if (it->getFileSystemType() == fat)
			flag = true;
	}

	return flag;
}

vector<Disk> DiskInfo::getDisks() const {
	return this->disks;
}