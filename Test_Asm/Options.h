// options.h
#pragma once
#include <windows.h>

struct Options
{
	bool ShowHiddenFiles;
	bool ConfirmOnDelete;
	bool UseQuickView;
};

// ќбъ€влени€ функций
void LoadOptionsFromIni();
void SaveOptionsToIni();
void InitConfigPath();

// √лобальна€ переменна€, объ€вление extern
extern Options GlobalOptions;
