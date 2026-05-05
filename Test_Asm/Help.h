#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include "Asm_Tools_Interface.h"

//------------------------------------------------------------------------------------------------------------
class Help
{
public:
	Help(unsigned short x_pos, unsigned short y_pos, unsigned short width, unsigned short height, CHAR_INFO* screen_buffer, unsigned short screen_width);
	void Draw();

private:
	unsigned short X_Pos, Y_Pos;
	unsigned short Width, Height;
	unsigned short Screen_Width;
	CHAR_INFO* Screen_Buffer;

	int Curr_File_Index;
	int Highlight_X_Offset;
	int Highlight_Y_Offset;
	std::wstring Current_Directory;
};
//------------------------------------------------------------------------------------------------------------