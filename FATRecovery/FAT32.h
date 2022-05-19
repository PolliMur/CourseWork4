#pragma once
#include "Reader.h"
#include "BootSector.h"
#include "Functions.h"
#include "File.h"

class FAT32
{
	Reader* reader;
	BootSector boot;
	UINT32* fatTable;
	UCHAR* rootDirectory;
	UINT32 rootDirectorySize;

public:
	FAT32(Reader* reader) : reader(reader) {};
	~FAT32() {};

	void init();
	void initBootSector();
	void initFatTable();
	void initRootDirectory();
	void searchDeletedFiles(vector<File*> &files);

	UINT64 readFileContent(File* file, UCHAR dstBuf[], UINT64 byteOff, UINT64 byteToRead);
	void restoreFile(File* file, wstring path);
};

