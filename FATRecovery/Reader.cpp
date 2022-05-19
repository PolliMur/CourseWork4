#include "Reader.h"

bool Reader::openDevice(wstring diskName) {
	diskName.at(diskName.length() - 1) = 0;
	diskHandle = ::CreateFile(diskName.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_DEVICE, NULL);
	if (diskHandle == INVALID_HANDLE_VALUE) return false;
	return true;
}

UINT64 Reader::readSector(UINT64 initialSector, UINT64 sectorSize, UINT64 bytesToRead, UCHAR* buffer) {
	if (diskHandle == INVALID_HANDLE_VALUE) return 0;

	UINT64 result = 0;
	UINT64 remainingBytes = bytesToRead % sectorSize;
	UINT64 tmpBytesToRead = bytesToRead - remainingBytes;

	LARGE_INTEGER tmp;
	tmp.QuadPart = initialSector * sectorSize;
	SetFilePointerEx(diskHandle, tmp, NULL, FILE_BEGIN);
	DWORD readBytes = 0;
	ReadFile(diskHandle, buffer, tmpBytesToRead, &readBytes, NULL);

	result = readBytes;

	if (remainingBytes) {
		UCHAR buf[4096] = {};
		ReadFile(diskHandle, buf, sectorSize, &readBytes, NULL);
		memcpy(buffer + tmpBytesToRead, buf, remainingBytes);
		result += remainingBytes;
	}

	return result;
}
