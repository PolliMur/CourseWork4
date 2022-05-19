#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <vector>
#include <Windows.h>
#include <iomanip>

using namespace std;

class Disk
{
	LPWSTR GUIDPath = 0;
	wstring label;
	wstring fileSystemType;
	double totalSize = 0;
	double totalFreeSize = 0;

public:
	Disk() {};
	Disk(LPCWSTR, wstring);
	Disk(const Disk&);
	~Disk(){};

	LPWSTR getGUIDPath() const;
	wstring getLabel() const;
	wstring getFileSystemType() const;
	double getTotalSize() const;
	double getTotalFreeSize() const;

	friend wostream& operator << (wostream& out, const Disk& disk);

	void init();
};

