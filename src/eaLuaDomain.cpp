#include <lua.hpp>
#include <exception>
#include "eaApplication.h"
#include "eaLuaDomain.h"

using namespace std;

eaLuaDomain::eaLuaDomain(const eaLua& L, const string& domain, std::shared_ptr<eaLuaDomain> owner) 
	:L(L), domain(domain), owner(owner)
{
	// get table
	// create _ENV
	lua_newtable(L);
	lua_pushstring(L, "domain");
	lua_pushstring(L, domain.c_str());
	lua_settable(L, -3);

	// set __index
	lua_newtable(L);
	lua_pushstring(L, "__index");
	if (owner == nullptr || owner->envTableRef == LUA_REFNIL)
		lua_getglobal(L, "_G");
	else
		lua_rawgeti(L, LUA_REGISTRYINDEX, owner->envTableRef);
	lua_settable(L, -3);

	lua_setmetatable(L, -2);

	envTableRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

eaLuaDomain::~eaLuaDomain()
{
	auto& L = eaApplication::GetLua();
	luaL_unref(L, LUA_REGISTRYINDEX, envTableRef);
}

int eaLuaDomain::DoString(string str)
{
	auto result = luaL_loadstring(L, str.c_str());
	if (result != LUA_OK)
	{
		string err = lua_tostring(L, -1);
		eaApplication::GetLogger().Error("Lua", "Failed to load a code string because: " + err);
		return -1;
	}
	lua_rawgeti(L, LUA_REGISTRYINDEX, envTableRef);
	lua_setupvalue(L, -2, 1);
	
	result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result != LUA_OK)
	{
		eaApplication::GetLogger().Error("Lua", "Failed to call a funciton. \n\tcurrent position at " + L.GetCurrentInfo() + "\n Code: " + str);
		return -1;
	}
	int resultCount = lua_gettop(L);

	return resultCount;
}

int eaLuaDomain::DoFile(string str)
{
	struct stat buffer;
	if (stat(str.c_str(), &buffer) != 0)
	{
		eaApplication::GetLogger().Error("Lua", "File " + str + " not exists");
		return -1;
	}

	auto result = luaL_loadfile(L, str.c_str()) == LUA_ERRSYNTAX;
	if (result != LUA_OK)
	{
		string err = lua_tostring(L, -1);
		eaApplication::GetLogger().Error("Lua", "Failed to load a file because: " + err);
		return -1;
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, envTableRef);
	lua_setupvalue(L, -2, 1);
	
	result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result != LUA_OK)
	{
		eaApplication::GetLogger().Error("Lua", "Failed to call a funciton. \n\tcurrent position at " + L.GetCurrentInfo());
		return -1;
	}
	int resultCount = lua_gettop(L);

	return resultCount;
}

std::shared_ptr<eaLuaDomain> eaLuaDomain::Create(const std::string& domain, std::shared_ptr<eaLuaDomain> owner)
{
	return std::shared_ptr<eaLuaDomain>(new eaLuaDomain(eaApplication::GetLua(), domain, owner));
}

void eaLuaDomain::Save(std::shared_ptr<eaProfileNode> node) 
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, envTableRef);
	
	lua_pushnil(L);
	while (lua_next(L, -2))
	{
		if (lua_isstring(L, -2))
		{
			// 跳过由C创建的带索引的table
			if (lua_getmetatable(L, -1))
			{
				lua_pushstring(L, "__index");
				lua_gettable(L, -2);
				bool isCTable = !lua_isnil(L, -1);
				lua_pop(L, 2);
				if (isCTable)
				{
					lua_pop(L, 1);
					continue;
				}
			}

			auto dataName = lua_tostring(L, -2);
			eaPropertyValue value = ToPropertyValue(L, -1);

			if (value != nullptr || lua_isnil(L, -2))
				node->Set(dataName, value);
			
		}
		lua_pop(L, 1);
	}
}

void eaLuaDomain::Load(std::shared_ptr<eaProfileNode> node) 
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, envTableRef);

	for (auto& data : node->GetData())
	{
		auto dataName = data.first;
		auto value = *reinterpret_pointer_cast<eaPropertyValue>(data.second);
		lua_pushstring(L, dataName.c_str());
		PushPropertyValue(L, value);
		lua_settable(L, -3);
	}
}