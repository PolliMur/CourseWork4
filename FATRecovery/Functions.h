#pragma once
#include <iostream>
#include <Windows.h>
#include <shlobj.h>

using namespace std;

void inputNumber(string message, std::istream& in, int& n, int begin, int end);
UINT32 converseToType(const UCHAR* bytes, int startBorder, int endBorder);
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
wstring BrowseFolder(wstring saved_path);
