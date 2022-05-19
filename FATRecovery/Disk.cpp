#include "Disk.h"

Disk::Disk(LPCWSTR path, wstring label) {
	this->GUIDPath = new WCHAR[MAX_PATH + 1];
	wcscpy(this->GUIDPath, path);
	this->label = label;
	init();
}

Disk::Disk(const Disk& other) {
	this->GUIDPath = new WCHAR[MAX_PATH + 1]; 
	wcscpy(this->GUIDPath, other.GUIDPath);
	this->label = other.label;
	this->fileSystemType = other.fileSystemType;
	this->totalSize = other.totalSize;
	this->totalFreeSize = other.totalFreeSize;
}

LPWSTR Disk::getGUIDPath() const{
	return this->GUIDPath;
}

wstring Disk::getLabel() const{
	return this->label;
}

wstring Disk::getFileSystemType() const{
	return this->fileSystemType;
}

double Disk::getTotalSize() const{
	return this->totalSize;
}

double Disk::getTotalFreeSize() const{
	return this->totalFreeSize;
}

void Disk::init() {
	LPWSTR fileSystem = new WCHAR[MAX_PATH + 1];
	fileSystem[0] = L'\0';
	GetVolumeInformationW(this->GUIDPath, NULL, NULL, NULL, NULL, NULL, fileSystem, MAX_PATH + 1);
		this->fileSystemType = fileSystem;

	ULARGE_INTEGER freeSize, size;
	freeSize.QuadPart = 0;
	size.QuadPart = 0;
	GetDiskFreeSpaceExW(this->label.c_str(), &freeSize, &size, NULL);
		this->totalFreeSize = (double) freeSize.QuadPart / (1024 * 1024 * 1024);
		this->totalSize = (double) size.QuadPart / (1024 * 1024 * 1024);
}

wostream& operator << (wostream& out, const Disk& disk) {
	out.setf(ios::left);
	out << setw(6) << disk.label;
	out << setw(20) << disk.fileSystemType;
	out << fixed << setprecision(1) << setw(7) << disk.totalSize << "/  ";
	out << fixed << setprecision(1) << setw(5) << disk.totalFreeSize;
	return out;
} 