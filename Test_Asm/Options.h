// options.h
#pragma once
#include <windows.h>

struct Options
{
	bool ShowHiddenFiles;
	bool ConfirmOnDelete;
	bool UseQuickView;
	bool WrapText;
	bool ShowFileExtensions;
};


// ќбъ€влени€ функций
void LoadOptionsFromIni();
void SaveOptionsToIni();

// √лобальна€ переменна€, объ€вление extern
extern Options GlobalOptions;
