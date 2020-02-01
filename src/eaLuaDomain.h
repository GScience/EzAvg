#pragma once
#include <string>
#include "eaLua.h"

struct lua_State;

struct eaLuaError : public std::exception
{
	eaLuaError() :std::exception("lua code error", 1)
	{
	}
};

class eaLuaDomain
{
	int envTableRef;
	const eaLua& L;
	const std::string domain;

	std::shared_ptr<eaLuaDomain> owner;

	eaLuaDomain(const eaLua& L, const std::string& domain, std::shared_ptr<eaLuaDomain> owner);
public:
	eaLuaDomain(const eaLuaDomain&) = delete;
	~eaLuaDomain();

	void DoString(std::string str);
	void DoFile(std::string str);

	int GetEnvTableRef()
	{
		return envTableRef;
	}

	static std::shared_ptr<eaLuaDomain> Create(const std::string& domain, std::shared_ptr<eaLuaDomain> owner = nullptr);
};