#pragma once
#include <string>
#include"Functions.h"
#include"BootSector.h"
#include<iomanip>

using namespace std;

class File
{
	wstring name;
	string modifyTime;
	UINT64 startCluster;
	UINT64 size;
	UINT64 startSector;
	UINT64 numberOfSectors;
	wstring directoryName;

public:
	File() {};
	~File() {};

	friend wostream& operator << (wostream& out, const File& file);

	void initFile(const UCHAR* record, BootSector boot);
	void initLongFileName(const UCHAR* record);
	void initShortFileName(const UCHAR* record);
	wstring getPartName(const UCHAR* record, int startBorder, int endBorder);
	void initModifyTime(const UCHAR* record);
	void initStartCluster(const UCHAR* record);
	void initSize(const UCHAR* record);
	void initSectors(BootSector boot);

	wstring getName();
	string getModifyTime();
	UINT64 getStartCluster();
	UINT64 getSize();
	UINT64 getStartSector();
	UINT64 getNumberOfSectors();
	wstring getDirectoryName();

	void setDirectoryName(wstring dirName);

};

