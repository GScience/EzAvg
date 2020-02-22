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
	std::shared_ptr<eaLuaDomain> domain;
	eaLuaBridge(std::shared_ptr<eaLuaDomain> owner, std::string type);

public:
	const std::string type;

	virtual ~eaLuaBridge();
	void Dispose();

	int GetObjRef()
	{
		return objRef;
	}

	void Save(std::shared_ptr<eaProfileNode> node)  override;
	void Load(std::shared_ptr<eaProfileNode> node)  override;
};