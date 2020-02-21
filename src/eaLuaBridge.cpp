#include "eaLuaBridge.h"
#include "eaApplication.h"
#include <lua.hpp>

eaLuaBridge::eaLuaBridge(std::shared_ptr<eaLuaDomain> owner, std::string type) :type(type)
{
	auto& L = eaApplication::GetLua();
	domain = eaLuaDomain::Create(type, owner);
	if (domain->DoFile(type + ".lua") == -1)
		eaApplication::GetLogger().Error("LuaBridge", "Failed to create an lua object with type " + type);
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


void eaLuaBridge::Save(eaProfileNode& node)
{

}

void eaLuaBridge::Load(eaProfileNode& node)
{

}
