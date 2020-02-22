#pragma once
#include <string>
#include "eaLua.h"
#include "eaSaveable.h"

struct lua_State;

class eaLuaDomain : public eaSaveable
{
	int envTableRef;
	const eaLua& L;
	const std::string domain;

	std::shared_ptr<eaLuaDomain> owner;

	eaLuaDomain(const eaLua& L, const std::string& domain, std::shared_ptr<eaLuaDomain> owner);
public:
	eaLuaDomain(const eaLuaDomain&) = delete;
	~eaLuaDomain();

	int DoString(std::string str);
	int DoFile(std::string str);

	int GetEnvTableRef()
	{
		return envTableRef;
	}

	void Save(eaProfileNode& node) override;
	void Load(eaProfileNode& node) override;

	static std::shared_ptr<eaLuaDomain> Create(const std::string& domain, std::shared_ptr<eaLuaDomain> owner = nullptr);
};