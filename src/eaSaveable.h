#pragma once

class eaSaveable
{
public:
	virtual void Save() = 0;
	virtual void Load() = 0;
};