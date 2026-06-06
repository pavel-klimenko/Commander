// options.cpp
#include "options.h"
#include <windows.h>

Options GlobalOptions; // определение переменной

const wchar_t* CONFIG_INI = L"commander.cfg";

void LoadOptionsFromIni()
{
	wchar_t buf[32];
	GetPrivateProfileStringW(L"Options", L"ShowHiddenFiles", L"0", buf, _countof(buf), CONFIG_INI);
	GlobalOptions.ShowHiddenFiles = (wcscmp(buf, L"1") == 0);

	GetPrivateProfileStringW(L"Options", L"ConfirmOnDelete", L"1", buf, _countof(buf), CONFIG_INI);
	GlobalOptions.ConfirmOnDelete = (wcscmp(buf, L"1") == 0);

	GetPrivateProfileStringW(L"Options", L"UseQuickView", L"1", buf, _countof(buf), CONFIG_INI);
	GlobalOptions.UseQuickView = (wcscmp(buf, L"1") == 0);

	GetPrivateProfileStringW(L"Options", L"WrapText", L"0", buf, _countof(buf), CONFIG_INI);
	GlobalOptions.WrapText = (wcscmp(buf, L"1") == 0);

	GetPrivateProfileStringW(L"Options", L"ShowFileExtensions", L"1", buf, _countof(buf), CONFIG_INI);
	GlobalOptions.ShowFileExtensions = (wcscmp(buf, L"1") == 0);
}

void SaveOptionsToIni()
{
	WritePrivateProfileStringW(L"Options", L"ShowHiddenFiles", GlobalOptions.ShowHiddenFiles ? L"1" : L"0", CONFIG_INI);
	WritePrivateProfileStringW(L"Options", L"ConfirmOnDelete", GlobalOptions.ConfirmOnDelete ? L"1" : L"0", CONFIG_INI);
	WritePrivateProfileStringW(L"Options", L"UseQuickView", GlobalOptions.UseQuickView ? L"1" : L"0", CONFIG_INI);
	WritePrivateProfileStringW(L"Options", L"WrapText", GlobalOptions.WrapText ? L"1" : L"0", CONFIG_INI);
	WritePrivateProfileStringW(L"Options", L"ShowFileExtensions", GlobalOptions.ShowFileExtensions ? L"1" : L"0", CONFIG_INI);
}

