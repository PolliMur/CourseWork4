#pragma once
#include <iterator>
#include <algorithm>
#include "DiskInfo.h"
#include "Functions.h"
#include "File.h"


class Interface
{
public:
	Interface() {};
	~Interface() {};

	void printInformation(const DiskInfo& info) const;
	Disk chooseDisk(const DiskInfo& info) const;

	void printFiles(vector<File*>& files);
	File* chooseFile(const vector<File*>& files) const;
};

