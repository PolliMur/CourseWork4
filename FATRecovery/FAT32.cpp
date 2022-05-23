#include "FAT32.h"

void FAT32::init() {
	cout << "File system is initializing...";
	initBootSector();
	initFatTable();
	initRootDirectory();
}

void FAT32::initBootSector() {
	UCHAR buffer[48];

	reader->readSector(0, 512, 48, buffer);

	boot.bytesPerSector = converseToType(buffer, 11, 12);
	boot.sectorsPerCluster = converseToType(buffer, 13, 13);
	boot.reservedAreaSize = converseToType(buffer, 14, 15);
	boot.numberOfFATs = converseToType(buffer, 16, 16);
	boot.numberOfSectors2 = converseToType(buffer, 19, 20);
	boot.numberOfSectors4 = converseToType(buffer, 32, 35);
	boot.sizeOfFat = converseToType(buffer, 36, 39);
	boot.numberOfUpdateFat = converseToType(buffer, 40, 41);
	boot.rootDirectoryCluster = converseToType(buffer, 44, 47);
}

void FAT32::initFatTable() {
	int numberOfFatRecords = 0;

	int numberOfActiveFat = boot.numberOfFATs, startFatSector = boot.reservedAreaSize;

	if ((boot.numberOfUpdateFat & 128) == 0) {
		numberOfActiveFat = boot.numberOfUpdateFat;
		numberOfActiveFat &= 15;
	}

	if (numberOfActiveFat != boot.numberOfFATs)
		startFatSector += numberOfActiveFat * boot.sizeOfFat;

	int currentSector = startFatSector + boot.sizeOfFat;
	bool isEmptyRecords = true;

	UCHAR* buffer = new UCHAR[boot.bytesPerSector];

	do {
		--currentSector;
		reader->readSector(currentSector, boot.bytesPerSector, boot.bytesPerSector, buffer);

		for (int i = 0; i < boot.bytesPerSector; i++) {
			if (buffer[i] != 0) {
				isEmptyRecords = false;
				break;
			}
		}

	} while (isEmptyRecords && currentSector != startFatSector);

	delete[] buffer;
	
	numberOfFatRecords = (currentSector - startFatSector + 1) * boot.bytesPerSector / 4;
	buffer = new UCHAR[numberOfFatRecords * 4];

	fatTable = new UINT32[numberOfFatRecords * 2];

	reader->readSector(startFatSector, boot.bytesPerSector,
		(currentSector - startFatSector + 1) * boot.bytesPerSector, buffer);

	for (int i = 0; i < numberOfFatRecords; ++i) {
		UINT32 temp = 0;

		for (int j = 3; j >= 0; --j) {
			temp = temp << 8;
			UCHAR c = buffer[i * 4 + j];
			temp += buffer[i * 4 + j];

		}

		fatTable[i] = temp;
	}

	delete[] buffer;
}

void FAT32::initRootDirectory() {
	UCHAR* buffer = new UCHAR[boot.bytesPerSector];
	bool stop = false;

	vector<int> rootDirectoryClusters;
	int currentCluster = boot.rootDirectoryCluster;

	while (currentCluster < 0xffffff7) {
		rootDirectoryClusters.push_back(currentCluster);
		currentCluster = fatTable[currentCluster];
	}

	vector<int>::iterator it = rootDirectoryClusters.begin();
	advance(it, rootDirectoryClusters.size() - 1);
	int currentSector = boot.reservedAreaSize + boot.sizeOfFat * boot.numberOfFATs;
	int noZeroSectors = 0;

	for (; noZeroSectors < 8; ++noZeroSectors) {
		reader->readSector(currentSector, boot.bytesPerSector, boot.bytesPerSector, buffer);
		for (int j = 0; j < boot.bytesPerSector; j += 32) {
			if (buffer[j] == 0) stop = true;
			else break;
		}
		if (stop) break;
		++currentSector;
	}

	delete[] buffer;

	rootDirectorySize = boot.bytesPerSector * boot.sectorsPerCluster; 
	rootDirectory = new UCHAR[rootDirectorySize];

	int rootDirectorySector = boot.reservedAreaSize + boot.numberOfFATs * boot.sizeOfFat;
	reader->readSector(rootDirectorySector, boot.bytesPerSector, rootDirectorySize, rootDirectory);
}

void FAT32::searchDeletedFiles(vector<File*>& files) {
	UINT32 offset = 0;
	TCHAR	fileName[MAX_PATH] = { 0 };

	while (offset < rootDirectorySize) {
		if (rootDirectory[offset] != 0xE5) {
			offset += 32;
			continue;
		}

		else {
			UINT32 firstEntry = offset;

			while (rootDirectory[offset + 11] == 0x0F) {
				offset += 32;
			}

			UINT32 dirEntry = offset;
			File *file = new File;

			if (rootDirectory[dirEntry] == 0xE5 && converseToType(rootDirectory + dirEntry, 28, 31) != 0
				&& converseToType(rootDirectory + dirEntry, 28, 31) != 0xFFFFFFFF) {
					if (firstEntry == dirEntry) {
						file->initShortFileName(rootDirectory + offset);
					}
					else {
						file->initLongFileName(rootDirectory + firstEntry);
					}

					file->initFile(rootDirectory + dirEntry, this->boot);
					if (rootDirectory[offset + 11] == 0x10 && rootDirectory[offset] != 0x2E) {
						searchDeletedFilesSubdirectory(files, file, L"root");
					}
					else {
						file->setDirectoryName(L"root");
						files.push_back(file);
					}

					while (rootDirectory[offset + 11] == 0x0f) offset += 32;
					offset += 32;
			}
			else {
				offset += 32;
			}


		}
	}
}

void FAT32::searchDeletedFilesSubdirectory(vector<File*>& files, File* fileDirectory, wstring dirName) {
	UINT32 offset = 0;
	UINT64 directorySector = fileDirectory->getStartSector();
	TCHAR	fileName[MAX_PATH] = { 0 };
	UINT32 directorySize = fileDirectory->getSize();
	UCHAR* directory = new UCHAR[directorySize];
	reader->readSector(directorySector, boot.bytesPerSector, directorySize, directory);

	while (offset < directorySize) {
		if (directory[offset] != 0xE5) {//|| rootDirectory[offset + 11] == 0x010) {
			offset += 32;
			continue;
		}

		else {
			UINT32 firstEntry = offset;

			while (directory[offset + 11] == 0x0F) {
				offset += 32;
			}

			UINT32 dirEntry = offset;
			File* file = new File;

			if (directory[dirEntry] == 0xE5 && (directory[dirEntry + 11] == 0x20 || directory[dirEntry + 11] == 0x10)  && converseToType(directory + dirEntry, 28, 31) != 0
				&& converseToType(directory + dirEntry, 28, 31) != 0xFFFFFFFF) {
				if (firstEntry == dirEntry) {
					file->initShortFileName(directory + offset);
				}
				else {
					file->initLongFileName(directory + firstEntry);
				}

				file->initFile(directory + dirEntry, this->boot);
				if (directory[offset + 11] == 0x10 && directory[offset] != 0x2E) {
					searchDeletedFilesSubdirectory(files, file, dirName + L"\\" + fileDirectory->getName());
				}
				else {
					file->setDirectoryName(dirName + L"\\" + fileDirectory->getName());
					files.push_back(file);
				}

				while (directory[offset + 11] == 0x0f) offset += 32;
				offset += 32;
			}
			else {
				offset += 32;
			}


		}
	}
}

UINT64 FAT32::readFileContent(File* file, UCHAR dstBuf[], UINT64 byteOff, UINT64 byteToRead) {
	UINT64 result = 0;
	UINT64 fileSize = file->getSize();
	UINT16 bytesPerSector = boot.bytesPerSector;

	if (byteOff > fileSize) {
		byteOff = fileSize;
	}

	if (byteToRead > fileSize - byteOff) {
		byteToRead = fileSize - byteOff;
	}

	UINT64 fileOffset = file->getStartSector() * bytesPerSector + byteOff;

	if (fileOffset % bytesPerSector != 0) {
		UCHAR buf[1024] = { 0 };
		UINT32 alignSize = fileOffset % bytesPerSector;
		reader->readSector(fileOffset / bytesPerSector, bytesPerSector, bytesPerSector, buf);

		result = bytesPerSector - alignSize;

		UINT32 copyBytes = byteToRead > bytesPerSector - alignSize ? 512 - alignSize : (UINT32)byteToRead;
		memcpy(dstBuf, buf + alignSize, copyBytes);
		fileOffset += bytesPerSector;

		if (byteToRead <= result) {
			return byteToRead;
		}

		byteToRead -= result;
	}

	result += reader->readSector(fileOffset / bytesPerSector, bytesPerSector, byteToRead, dstBuf + result);
	
	return result;
}

void FAT32::restoreFile(File* file, wstring path) {
	char* tmpBuf = (char*)malloc(1024 * 1024);

	memset(tmpBuf, 0, 1024 * 1024);

	UINT64	tmpFileSize = file->getSize();
	UINT64	tmpBytesRead = 0;
	FILE* fp = NULL;

	wstring fullPath = path + L"\\" + file->getName();

	_wfopen_s(&fp, fullPath.c_str(), L"wb");
	if (fp == NULL) {
		cout << "Failed to restore file!\n";
	}

	else {
		while (tmpBytesRead < tmpFileSize)
		{
			UINT64  tmpVal = readFileContent(file, (UCHAR*)tmpBuf, tmpBytesRead, 1024 * 1024);
			if (tmpVal == 0)
			{
				break;
			}

			tmpBytesRead += tmpVal;
			fwrite(tmpBuf, 1, (DWORD)tmpVal, fp);
		}
		cout << "File successfully restored!\n";
	}

	free(tmpBuf);
	fclose(fp);
}