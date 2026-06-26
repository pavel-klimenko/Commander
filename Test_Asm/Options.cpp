// options.cpp
#include "options.h"
#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


Options GlobalOptions; // определение переменной

wchar_t CONFIG_INI[MAX_PATH];


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

void InitConfigPath()
{
    // Получаем путь к EXE
    GetModuleFileNameW(nullptr, CONFIG_INI, MAX_PATH);

    // Убираем имя exe
    PathRemoveFileSpecW(CONFIG_INI);

    // Добавляем имя INI
    wcscat_s(CONFIG_INI, L"\\commander.ini");

    // Если файла нет — создаём
    DWORD attr = GetFileAttributesW(CONFIG_INI);
    if (attr == INVALID_FILE_ATTRIBUTES)
    {
        HANDLE h = CreateFileW(CONFIG_INI, GENERIC_WRITE, 0, nullptr,
            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE)
        {
            const char* header = "[Options]\r\n";
            DWORD written;
            WriteFile(h, header, (DWORD)strlen(header), &written, nullptr);
            CloseHandle(h);
        }
    }
}


//TODO выгрузка в GIT