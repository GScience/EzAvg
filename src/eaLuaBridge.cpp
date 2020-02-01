#include "eaLuaBridge.h"
#include "eaApplication.h"
#include <lua.hpp>

eaLuaBridge::eaLuaBridge(std::shared_ptr<eaLuaDomain> owner, std::string name)
{
	auto& L = eaApplication::GetLua();
	domain = eaLuaDomain::Create(name, owner);
	domain->DoFile(name + ".lua");
	objRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

eaLuaBridge::~eaLuaBridge()
{
	Dispose();
}

void eaLuaBridge::Dispose()
{
	if (objRef == LUA_REFNIL)
		return;

	auto & L = eaApplication::GetLua();

	luaL_unref(L, LUA_REGISTRYINDEX, objRef);
	objRef = LUA_REFNIL;
}


void eaLuaBridge::Save()
{
}

void eaLuaBridge::Load()
{
}
