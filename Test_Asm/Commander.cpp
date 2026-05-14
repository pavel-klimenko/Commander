#include "Commander.h"

// AMenu_Item
//------------------------------------------------------------------------------------------------------------
AMenu_Item::AMenu_Item(unsigned short x_pos, unsigned short y_pos, unsigned short len, const wchar_t *key, const wchar_t *name)
: X_Pos(x_pos), Y_Pos(y_pos), Len(len), Key(key), Name(name)
{
}
//------------------------------------------------------------------------------------------------------------
void AMenu_Item::Draw(CHAR_INFO *screen_buffer, unsigned short screen_width)
{
	int key_str_len;

	SText_Pos key_pos(X_Pos, Y_Pos, screen_width, 0x07);
	key_str_len = Draw_Text(screen_buffer, key_pos, Key);

	SText_Pos name_pos(X_Pos + key_str_len, Y_Pos, screen_width, 0xb0);
	Draw_Limited_Text(screen_buffer, name_pos, Name, Len);
}
//------------------------------------------------------------------------------------------------------------




// AsCommander
//------------------------------------------------------------------------------------------------------------
AsCommander::~AsCommander()
{
	// Restore the original active screen buffer.
	if (!SetConsoleActiveScreenBuffer(Std_Output_Handle))
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());

	delete Left_Panel;
	delete Right_Panel;
	delete Screen_Buffer;
}
//------------------------------------------------------------------------------------------------------------
bool AsCommander::Init()
{
	SMALL_RECT srctWriteRect;
	int screen_buffer_size;
	wchar_t curr_dir[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, curr_dir);

	Std_Input_Handle = GetStdHandle(STD_INPUT_HANDLE);
	Std_Output_Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	Screen_Buffer_Handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CONSOLE_TEXTMODE_BUFFER, 0);
	if (Std_Input_Handle == INVALID_HANDLE_VALUE || Std_Output_Handle == INVALID_HANDLE_VALUE || Screen_Buffer_Handle == INVALID_HANDLE_VALUE)
	{
		printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
		return false;
	}

	// Make the new screen buffer the active screen buffer.
	if (!SetConsoleActiveScreenBuffer(Screen_Buffer_Handle))
	{
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		return false;
	}

	if (!GetConsoleScreenBufferInfo(Screen_Buffer_Handle, &Screen_Buffer_Info))
	{
		printf("GetConsoleScreenBufferInfo failed - (%d)\n", GetLastError());
		return false;
	}

	screen_buffer_size = (int)Screen_Buffer_Info.dwSize.X * (int)Screen_Buffer_Info.dwSize.Y;
	Screen_Buffer = new CHAR_INFO[screen_buffer_size];
	memset(Screen_Buffer, 0, screen_buffer_size * sizeof(CHAR_INFO));

	// Set the destination rectangle.

	srctWriteRect.Top = 10;    // top lt: row 10, col 0
	srctWriteRect.Left = 0;
	srctWriteRect.Bottom = 11; // bot. rt: row 11, col 79
	srctWriteRect.Right = 79;

	int half_width = Screen_Buffer_Info.dwSize.X / 2;
	Left_Panel = new APanel(0, 0, half_width, Screen_Buffer_Info.dwSize.Y - 2, Screen_Buffer, Screen_Buffer_Info.dwSize.X);
	Right_Panel = new APanel(half_width, 0, half_width, Screen_Buffer_Info.dwSize.Y - 2, Screen_Buffer, Screen_Buffer_Info.dwSize.X);

	Build_Menu();

	Left_Panel->Get_Directory_Files(std::wstring(curr_dir) );

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsCommander::Run()
{
	unsigned long records_count;
	INPUT_RECORD input_record[128];

	Can_Run = true;
	Need_Redraw = true;

	while (Can_Run)
	{
		if (PeekConsoleInput(Std_Input_Handle, input_record, 128, &records_count) )
		{
			if (ReadConsoleInput(Std_Input_Handle, input_record, 1, &records_count) )
			{
				if (records_count != 0)
				{
					if (input_record[0].EventType == KEY_EVENT && input_record[0].Event.KeyEvent.bKeyDown)
					{
						switch (input_record[0].Event.KeyEvent.wVirtualKeyCode)
						{
						case VK_F1:
							Show_Help_Window();
							while (true)
							{
								INPUT_RECORD rec;
								DWORD cnt;

								ReadConsoleInput(Std_Input_Handle, &rec, 1, &cnt);

								if (rec.EventType == KEY_EVENT &&
									rec.Event.KeyEvent.bKeyDown &&
									rec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
								{
									Need_Redraw = true; // перерисовать панели
									break;
								}
							}
							break;
	
						case VK_F3:
							View_File();
							Need_Redraw = true;
							break;

						case VK_F7:
							Make_Directory();
							Need_Redraw = true;
							break;
						case VK_F8:
							Delete_Selected();
							Need_Redraw = true;
							break;
						case VK_F10:
							Can_Run = false;
							break;

						case VK_UP:
							Left_Panel->Move_Highlight(true);
							Need_Redraw = true;
							break;

						case VK_DOWN:
							Left_Panel->Move_Highlight(false);
							Need_Redraw = true;
							break;

						case VK_RETURN:
							Left_Panel->On_Enter();
							Need_Redraw = true;
							break;
						}
					}
				}
			}
		}

		if (Need_Redraw)
		{
			if (! Draw() )
				return;

			Need_Redraw = false;
		}

		Sleep(2);
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsCommander::Draw()
{
	int i;
	COORD screen_buffer_pos{};

	//SPos pos(1, 1, Screen_Buffer_Info.dwSize.X, 0);
	//CHAR_INFO symbol{};

	//symbol.Char.UnicodeChar = L'X';
	//Show_Colors(Screen_Buffer, pos, symbol);

	Left_Panel->Draw();
	Right_Panel->Draw();

	for (i = 0; i < 10; i++)
	{
		if (Menu_Items[i] != 0)
			Menu_Items[i]->Draw(Screen_Buffer, Screen_Buffer_Info.dwSize.X);
	}

	if (!WriteConsoleOutput(Screen_Buffer_Handle, Screen_Buffer, Screen_Buffer_Info.dwSize, screen_buffer_pos, &Screen_Buffer_Info.srWindow))
	{
		printf("WriteConsoleOutput failed - (%d)\n", GetLastError());
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsCommander::Add_Next_Menu_Item(int &index, int &x_pos, int x_step, const wchar_t *key, const wchar_t *name)
{
	Menu_Items[index++] = new AMenu_Item(x_pos, Screen_Buffer_Info.dwSize.Y - 1, 12, key, name);
	x_pos += x_step;

	if (index == 2)
		--x_pos;
}
//------------------------------------------------------------------------------------------------------------
void AsCommander::Build_Menu()
{
	int index = 0;
	int x_pos = 0;
	int x_step = Screen_Buffer_Info.dwSize.X / 10;

	Add_Next_Menu_Item(index, x_pos, x_step, L"F1", L"Help");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F2", L"UserMenu");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F3", L"View");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F4", L"Edit");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F5", L"Copy");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F6", L"RenMov");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F7", L"MakeDir");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F8", L"Delete");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F9", L"Config");
	Add_Next_Menu_Item(index, x_pos, x_step, L"F10", L"Quit");
}
//------------------------------------------------------------------------------------------------------------

void AsCommander::Show_Help_Window()
{
	int w = 50;
	int h = 20;
	int x = (Screen_Buffer_Info.dwSize.X - w) / 2;
	int y = (Screen_Buffer_Info.dwSize.Y - h) / 2;

	Help help(x, y, w, h, Screen_Buffer, Screen_Buffer_Info.dwSize.X);

	help.Draw();

	// Пишем текст
	SText_Pos pos(x + 2, y + 2, Screen_Buffer_Info.dwSize.X, 0x1F);

	Draw_Text(Screen_Buffer, pos, L"F1 – Show help");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F2 – User menu");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F3 – View file");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F4 – Edit file");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F5 – Copy");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F6 – Rename / Move");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F7 – Create directory");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F8 – Delete");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F9 – Configuration");
	pos.Y_Pos++;
	Draw_Text(Screen_Buffer, pos, L"F10 – Quit");

	WriteConsoleOutput(Screen_Buffer_Handle, Screen_Buffer,
		Screen_Buffer_Info.dwSize, { 0,0 }, &Screen_Buffer_Info.srWindow);
}

void AsCommander::View_File()
{
	const auto& files = Left_Panel->Get_Files();
	int index = Left_Panel->Get_Current_Index();

	if (index < 0 || index >= files.size())
		return;

	AFile_Descriptor* file = files[index];

	if (file->Attributes & FILE_ATTRIBUTE_DIRECTORY)
		return;

	std::wstring full_path = Left_Panel->Get_Current_Directory() + L"\\" + file->File_Name;

	// Открываем файл через _wfopen — работает ВСЕГДА
	FILE* f = nullptr;
	if (_wfopen_s(&f, full_path.c_str(), L"rt, ccs=UTF-8") != 0 || !f)
	{
		MessageBoxW(NULL, (L"Cannot open file:\n" + full_path).c_str(), L"Error", MB_OK);
		return;
	}


	std::wstring content;
	wchar_t buffer[1024];

	while (fgetws(buffer, 1024, f))
		content += buffer;

	fclose(f);

	MessageBoxW(NULL, content.c_str(), file->File_Name.c_str(), MB_OK);
}

void AsCommander::Make_Directory()
{
	// Размеры окна ввода
	int w = 40;
	int h = 6;

	int x = (Screen_Buffer_Info.dwSize.X - w) / 2;
	int y = (Screen_Buffer_Info.dwSize.Y - h) / 2;

	// Рисуем рамку
	Help input_box(x, y, w, h, Screen_Buffer, Screen_Buffer_Info.dwSize.X);
	input_box.Draw();

	// Текст приглашения
	SText_Pos pos(x + 2, y + 1, Screen_Buffer_Info.dwSize.X, 0x1F);
	Draw_Text(Screen_Buffer, pos, L"Enter directory name:");

	// Поле ввода
	std::wstring name;
	int max_len = w - 4;

	WriteConsoleOutput(Screen_Buffer_Handle, Screen_Buffer,
		Screen_Buffer_Info.dwSize, { 0,0 }, &Screen_Buffer_Info.srWindow);

	// Чтение клавиш
	while (true)
	{
		INPUT_RECORD rec;
		DWORD cnt;
		ReadConsoleInput(Std_Input_Handle, &rec, 1, &cnt);

		if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown)
		{
			wchar_t ch = rec.Event.KeyEvent.uChar.UnicodeChar;
			WORD vk = rec.Event.KeyEvent.wVirtualKeyCode;

			// Enter → создать каталог
			if (vk == VK_RETURN)
				break;

			// Esc → отмена
			if (vk == VK_ESCAPE)
				return;

			// Backspace
			if (vk == VK_BACK && !name.empty())
			{
				name.pop_back();
			}
			// Печатаемые символы
			else if (ch >= 32 && ch < 127 && name.size() < max_len)
			{
				name.push_back(ch);
			}

			// Перерисовать поле ввода
			std::wstring line = name;
			while (line.size() < max_len) line.push_back(L' ');

			SText_Pos pos2(x + 2, y + 4, Screen_Buffer_Info.dwSize.X, 0x1E);
			Draw_Text(Screen_Buffer, pos2, line.c_str());

			WriteConsoleOutput(Screen_Buffer_Handle, Screen_Buffer,
				Screen_Buffer_Info.dwSize, { 0,0 }, &Screen_Buffer_Info.srWindow);
		}
	}

	// Если имя пустое — ничего не делаем
	if (name.empty())
		return;

	// Формируем путь
	const std::wstring& base = Left_Panel->Get_Current_Directory();

	wchar_t full_path[260] = { 0 };
	Build_Full_Path(base.c_str(), name.c_str(), full_path);


	// Создаём каталог
	if (CreateDirectoryW(full_path, NULL)) {
		MessageBoxW(NULL, L"Directory created.", L"F7 MakeDir", MB_OK);
	} else {
		MessageBoxW(NULL, full_path, L"Cannot create directory", MB_OK);
	}

	// Обновляем панель
	Left_Panel->Get_Directory_Files(Left_Panel->Get_Current_Directory());
}

void AsCommander::Delete_Selected()
{
	AFile_Descriptor* file = Left_Panel->Get_Selected_File();
	if (file == nullptr)
		return;

	std::wstring path = file->Full_Path;

	// Подтверждение
	std::wstring msg = L"Delete:\n";
	msg += path;
	msg += L"\n\nAre you sure?";

	int res = MessageBoxW(NULL, msg.c_str(), L"Delete", MB_YESNO | MB_ICONQUESTION);
	if (res != IDYES)
		return;

	DWORD attr = GetFileAttributesW(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES)
	{
		MessageBoxW(NULL, L"Cannot get file attributes.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	bool ok = false;

	if (attr & FILE_ATTRIBUTE_DIRECTORY)
	{
		// Удаляем директорию (включая непустую)
		ok = Delete_Directory_Recursive(path);
	}
	else
	{
		// Удаляем файл
		ok = DeleteFileW(path.c_str());
	}

	if (!ok)
	{
		MessageBoxW(NULL, L"Delete failed.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Обновляем панель
	Left_Panel->Get_Directory_Files(Left_Panel->Get_Current_Directory());
}

bool AsCommander::Delete_Directory_Recursive(const std::wstring& dir)
{
	WIN32_FIND_DATAW ffd;
	std::wstring search = dir + L"\\*";
	HANDLE hFind = FindFirstFileW(search.c_str(), &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		std::wstring name = ffd.cFileName;

		if (name == L"." || name == L"..")
			continue;

		std::wstring full = dir + L"\\" + name;

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!Delete_Directory_Recursive(full))
			{
				FindClose(hFind);
				return false;
			}
		}
		else
		{
			if (!DeleteFileW(full.c_str()))
			{
				FindClose(hFind);
				return false;
			}
		}

	} while (FindNextFileW(hFind, &ffd));

	FindClose(hFind);

	// Теперь удаляем саму директорию
	return RemoveDirectoryW(dir.c_str());
}

