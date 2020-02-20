#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "eaApplication.h"
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

void PopMsgBox(string message)
{
	auto scene = eaApplication::instance->GetActiveScene();
	scene->PopScene("popScene/MessageBox.scene", map<eaPropertyValue, eaPropertyValue>
	{ 
		{"message"s, message} 
	});
}

void eaLogger::Log(eaLogLevel logLevel, string category, string message)
{
	switch (logLevel)
	{
	case LevelInfo:
		cout << "[Info][" << category << "]" << message << endl;
		break;
	case LevelWarning:
		cout << "[Warning][" << category << "]" << message << endl;
		break;
	case LevelError:
		PopMsgBox(category + "\n" + message);
		cout << "[Error][" << category << "]" << message << endl;
		break;
	case LevelCrash:
		PopMsgBox(category + "\n" + message);
		cout << "[CRACH][" << category << "]" << message << endl;
		throw exception(message.c_str());
		break;
	}
}