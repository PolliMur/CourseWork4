#include "File.h"

void File::initFile(const UCHAR* record, BootSector boot) {
	initModifyTime(record);
	initStartCluster(record);
	initSize(record);
	initSectors(boot);
}

void File::initLongFileName(const UCHAR* record) {
	int offset = 0;
	wstring tmpName;

	while (record[offset + 11 + 32] == 0x0f) offset += 32;


	while (offset != -32) {
		tmpName += getPartName(record + offset, 1, 10);
		tmpName += getPartName(record + offset, 14, 25);
		tmpName += getPartName(record + offset, 28, 31);

		tmpName.erase(remove(tmpName.begin(), tmpName.end(), 0xffff), tmpName.end());

		offset -= 32;
	}

	this->name = tmpName;
}

wstring File::getPartName(const UCHAR* record, int startBorder, int endBorder)
{
	UCHAR* buffer = new UCHAR[12];
	wstring part;

	memcpy(buffer, record + startBorder, endBorder - startBorder + 1);

	int i = 0;
	for (int offset = startBorder; offset < endBorder + 1; offset += 2, ++i) {
		part.push_back(converseToType(record, offset, offset + 1));
	}

	delete[] buffer;

	return part;
}

void File::initShortFileName(const UCHAR* record) {
	char* nameBuffer = (char*)calloc(13, sizeof(char));
	copy(record, record + 11, nameBuffer);
	nameBuffer[0] = '$';
	string fileName(nameBuffer);

	wstring wName(fileName.begin(), fileName.end());
	this->name = wName;

	wstring::iterator it = this->name.begin();
	advance(it, 8);
	this->name.insert(it, '.');
	this->name.erase(remove(this->name.begin(), this->name.end(), ' '), this->name.end());

	free(nameBuffer);
}

string formatDatePart(int number) {
	if (number < 10) {
		return "0" + to_string(number);
	}

	return to_string(number);
}

string getFormattedDate(int year, int month, int day, int hours, int minutes, int seconds) {
	return formatDatePart(day) + "/" +
		formatDatePart(month) + "/" +
		to_string(year + 1980) + "  " +
		formatDatePart(hours) + ":" +
		formatDatePart(minutes) + ":" +
		formatDatePart(seconds);
}

void File::initModifyTime(const UCHAR* record) {
	UINT32 date = converseToType(record, 24, 25);
	UINT32 time = converseToType(record, 22, 23);

	int hours = (time & 0xF800) >> 11;
	int minutes = (time & 0x7E0) >> 5;
	int	seconds = (time & 0x1F);
	int year = (date & 0xFE00) >> 9;
	int month = (date & 0x1E0) >> 5;
	int day = (date & 0x1F);

	this->modifyTime = getFormattedDate(year, month, day, hours, minutes, seconds);
}

void File::initStartCluster(const UCHAR* record) {
	UINT32 result = ((converseToType(record, 20, 21)) << 16) & (0xFFFF0000);
	result = result | (converseToType(record, 26, 27) & 0xFFFF);
	if (result == 0x0FFFFFFF || result == 0x0FFFFFF8)
	{
		this->startCluster = 0;
	}
    this->startCluster = result;
}

void File::initSectors(BootSector boot) {
	UINT32	baseSector = boot.numberOfFATs * boot.sizeOfFat + boot.reservedAreaSize;
	UINT32	clusterSize = boot.sectorsPerCluster;

	startSector = startCluster * boot.sectorsPerCluster + baseSector- boot.rootDirectoryCluster * boot.sectorsPerCluster;
	UINT32 fileSize = size;
	fileSize += 511;
	fileSize = fileSize & ~(511);
	numberOfSectors = fileSize >> 9;
}

void File::initSize(const UCHAR* record) {
	this->size = converseToType(record, 28, 31);
}

wstring File::getName() {
	return name;
}

string File::getModifyTime() {
	return modifyTime;
}

UINT64 File::getStartCluster() {
	return startCluster;
}

UINT64 File::getSize() {
	return size;
}

UINT64 File::getStartSector() {
	return startSector;
}

UINT64 File::getNumberOfSectors() {
	return numberOfSectors;
}

wstring File::getDirectoryName() {
	return directoryName;
}

void File::setDirectoryName(wstring dirName) {
	this->directoryName = dirName;
}

wostream& operator << (wostream& out, const File& file) {
	wstring formattedFileName = file.name;

	if (file.name.length() > 65) {
		formattedFileName = L"";

		int lastDotIndex = -1;

		for (int i = file.name.length() - 1; i >= 0; i--) {
			if (file.name[i] == L'.') {
				lastDotIndex = i;

				break;
			}
		}

		int extLength = file.name.length() - 1 - lastDotIndex;

		for (int i = 0; i < lastDotIndex && formattedFileName.length() + extLength <= 65; i++) {
			formattedFileName += file.name[i];
		}

		formattedFileName += L"...";

		for (int i = lastDotIndex; i < file.name.length(); i++) {
			formattedFileName += file.name[i];
		}
	}

	out.setf(ios::left);
	out << setw(70) << formattedFileName;
	out << setw(31) << wstring(file.modifyTime.begin(), file.modifyTime.end());
	out << setw(10) << (UINT64) ceil((double) file.size/ 1024);

	return out;
}