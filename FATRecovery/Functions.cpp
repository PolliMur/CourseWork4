#include "Functions.h"

void inputNumber(string message, std::istream& in, int& n, int begin, int end)
{
	cout << message;
	bool flag = true;
	do
	{
		if (flag == 0) cout << "\nReenter: ";
		flag = true;
		in >> n;
		while (in.peek() == ' ') in.get();
		if (!in.good() || in.peek() != '\n') flag = false;

		if (n < begin || n > end) flag = 0;

		in.clear();
		rewind(stdin);
	} while (!flag);
}

UINT32 converseToType(const UCHAR* bytes, int startBorder, int endBorder)
{
	UINT32 number = 0;
	for (int i = endBorder; i >= startBorder; --i) {
		number = number << 8;
		number += bytes[i];
	}
	return number;
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{

	if (uMsg == BFFM_INITIALIZED)
	{
		LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
		::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
	}

	return 0;
}

wstring BrowseFolder(wstring saved_path)
{
	TCHAR path[MAX_PATH];

	wstring wsaved_path(saved_path.begin(), saved_path.end());
	const wchar_t* path_param = wsaved_path.c_str();

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = (L"Browse for folder...");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)path_param;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		//get the name of the folder and put it in path
		SHGetPathFromIDList(pidl, path);

		//free memory used
		IMalloc* imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}

		return path;
	}

	return L"";
}