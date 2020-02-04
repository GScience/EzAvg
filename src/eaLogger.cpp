#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "ealogger.h"

using namespace std;

eaLogger::eaLogger()
{
#ifdef _WIN32    
	SetConsoleOutputCP (65001);    
	CONSOLE_FONT_INFOEX info = { 0 };   
	info.cbSize = sizeof(info);    
	info.dwFontSize.Y = 16;  
	info.FontWeight = FW_NORMAL;   
	wcscpy_s(info.FaceName, L"Consolas");    
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
#endif
}

void eaLogger::Log(string category, string message)
{
	cout << "[" << category << "]" << message << endl;
}