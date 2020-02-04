#pragma once
#include <string>

class eaLogger
{
public:
	eaLogger();
	eaLogger(const eaLogger&) = delete;

	void Log(std::string category, std::string message);
};