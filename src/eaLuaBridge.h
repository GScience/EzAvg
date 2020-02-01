#pragma once
#include <string>
#include <memory>
#include "eaSaveable.h"

class eaLuaDomain;

/*
与Lua对象绑定的cpp类型
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