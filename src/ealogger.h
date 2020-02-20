#pragma once
#include <string>

enum eaLogLevel
{
	LevelInfo, LevelWarning, LevelError, LevelCrash
};
class eaLogger
{
public:
	eaLogger();
	eaLogger(const eaLogger&) = delete;

	void Log(eaLogLevel logLevel, std::string category, std::string message);
	void Info(std::string category, std::string message)
	{
		Log(LevelInfo, category, message);
	}
	void Warning(std::string category, std::string message)
	{
		Log(LevelWarning, category, message);
	}
	void Error(std::string category, std::string message)
	{
		Log(LevelError, category, message);
	}
	void Crash(std::string category, std::string message)
	{
		Log(LevelCrash, category, message);
	}
};