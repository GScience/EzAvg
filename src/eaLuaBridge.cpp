#include "eaLuaBridge.h"
#include "eaApplication.h"
#include <memory>
#include <lua.hpp>

using namespace std;

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


void eaLuaBridge::Save(std::shared_ptr<eaProfileNode> node) 
{
	auto luaNode = node->Set("LuaObj");

	if (objRef == LUA_REFNIL)
		return;

	auto & L = eaApplication::GetLua();
	
	// 保存Lua对象的所有值
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);

	lua_pushnil(L);
	while (lua_next(L, -2))
	{
		if (lua_isstring(L, -2))
		{
			auto name = lua_tostring(L, -2);
			eaPropertyValue value = ToPropertyValue(L, -1);

			if (value != nullptr || lua_isnil(L, -2))
				luaNode->Set(name, value);
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 2);
}

void eaLuaBridge::Load(std::shared_ptr<eaProfileNode> node) 
{
	if (objRef == LUA_REFNIL)
		return;

	auto & L = eaApplication::GetLua();

	// 加载Lua对象的所有值
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);

	auto luaNode = node->Get<eaProfileNode>("LuaObj");
	for (auto& data : luaNode->GetData())
	{
		lua_pushstring(L, data.first.c_str());
		PushPropertyValue(L, *reinterpret_pointer_cast<eaPropertyValue>(data.second));
		lua_settable(L, -3);
	}
	lua_pop(L, 1);
}
