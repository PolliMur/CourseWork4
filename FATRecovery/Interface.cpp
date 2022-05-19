#include "Interface.h"

void Interface::printInformation(const DiskInfo& info) const{
	wcout.setf(ios::left);
	wcout << setw(3) << "#";
	wcout << setw(6) << "Disk";
	wcout << setw(20) << "File System Type";
	wcout << setw(25) << "Total  /  Free Size (GB)";
	wcout << endl;

	string divider(54, '-');
	cout << divider << endl;

	vector<Disk> disks = info.getDisks();

	auto it = disks.begin();
	int i = 1;
	for (it = disks.begin(); it != disks.end(); ++it, ++i) {
		wcout << setw(3) << i << *it << endl;
	}

	cout << "\nPress 0 to exit...\n\n";
}

Disk Interface::chooseDisk(const DiskInfo& info) const {
	int i;
	inputNumber("Choose disk: ", cin, i, 0, info.getDisks().size());

	if (i == 0)
		exit(0);

	return info.getDisks()[i - 1];
}

void Interface::printFiles(const vector<File*>& files) const {
	wcout.setf(ios::left);
	wcout << setw(4) << "#";
	wcout << setw(70) << "File";
	wcout << setw(31) << "Date modified";
	wcout << setw(10) << "Size";
	wcout << endl;

	string divider(120, '-');
	cout << divider << endl;

	auto it = files.begin();
	int i = 1;

	for (it = files.begin(); it != files.end(); ++it, ++i) {
		wcout << setw(4) << i << **it << endl;
	}

	cout << "\nPress 0 to choose new disk...\n\n";
}

File* Interface::chooseFile(const vector<File*>& files) const {
	int i;
	inputNumber("Choose file to recover: ", cin, i, 0, files.size());

	return i == 0 ? NULL : files[i - 1];
}