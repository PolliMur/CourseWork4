#include <iostream>
#include "Interface.h"
#include "FAT32.h"

int main() {
    wcout.imbue(locale("rus_rus.866"));
    wcin.imbue(locale("rus_rus.866"));

	DiskInfo diskInfo;
	diskInfo.searchDisks();

	Interface i;

    while (true) {
        system("cls");
        i.printInformation(diskInfo);

        if (!diskInfo.thereIsFAT32()) {
            cout << "\nFAT32 file system not found. The program terminates!\n\n";

            system("pause");

            return EXIT_SUCCESS;
        }
        else {
            Disk disk = i.chooseDisk(diskInfo);

            if (disk.getFileSystemType() != L"FAT32") {
                cout << "Type of file system is not FAT32. Choose another disk!\n\n";

                system("pause");

                continue;
            }
            else {
                Reader reader;
                if (!reader.openDevice(disk.getGUIDPath())) {
                    cout << "Error of opening disk\n";
                    continue;
                }

                FAT32 fat(&reader);
                fat.init();

                vector<File*> files;
                fat.searchDeletedFiles(files);

                wstring path = L"";
                while (true) {
                    system("cls");
                    i.printFiles(files);
                    File* file = i.chooseFile(files);

                    if (file == NULL) break;

                    path = BrowseFolder(path);

                    fat.restoreFile(file, path);
                    system("pause");
                }
                system("cls");
            }
        }
    }
    system("pause");
	return 0;
}