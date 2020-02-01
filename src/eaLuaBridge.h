#pragma once
#include <string>
#include <memory>
#include "eaSaveable.h"

class eaLuaDomain;

/*
��Lua����󶨵�cpp����
*/
class eaLuaBridge : public eaSaveable
{
protected:
	int objRef;
	const std::string type;
	std::shared_ptr<eaLuaDomain> domain;
	eaLuaBridge(std::shared_ptr<eaLuaDomain> owner, std::string type);

public:
	virtual ~eaLuaBridge();
	void Dispose();

	int GetObjRef()
	{
		return objRef;
	}

	void Save() override;
	void Load() override;
};