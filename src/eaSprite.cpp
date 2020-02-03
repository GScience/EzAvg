#include <lua.hpp>
#include <iostream>
#include "eaSprite.h"
#include "eaApplication.h"

using namespace std;

void eaSprite::Update()
{
	for (auto behaviour = behaviours.begin(); behaviour != behaviours.end();)
	{
		if (!(*behaviour)->IsEnabled())
			behaviour = behaviours.erase(behaviour);
		else
		{
			(*behaviour)->Update();
			++behaviour;
		}
	}
}

std::shared_ptr<eaSpriteBehaviour> eaSprite::AddBehaviour(std::string name, std::string type)
{
	auto newBehaviour = make_shared<eaSpriteBehaviour>(this, name, type);
	behaviours.push_back(newBehaviour);
	newBehaviour->Start();

	return newBehaviour;
}

std::shared_ptr<eaSpriteBehaviour> eaSprite::GetBehaviour(std::string name)
{
	for (auto behaviour : behaviours)
		if (behaviour->name == name)
			return behaviour;
	return nullptr;
}

eaSprite::eaSprite()
{
	propertyBinder["enabled"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return enabled;
		},
		[&](eaPropertyValue value)
		{
			enabled = value.ToBoolean();
		}
	);
	propertyBinder["size"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return { size.width,size.height };
		},
		[&](eaPropertyValue value)
		{
			size.width = value[1];
			size.height = value[2];
			OnResize();
		}
	);
	propertyBinder["pos"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return { pos.x,pos.y };
		},
		[&](eaPropertyValue value)
		{
			pos.x = value[1];
			pos.y = value[2];
			OnMove();
		}
	);
	propertyBinder["alpha"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return alpha;
		},
		[&](eaPropertyValue obj)
		{
			alpha = obj.ToNumber();
		}
		);
}

static eaPropertyValue ToPropertyValue(lua_State* L, int index);

static eaPropertyValue::eaTable ToTableValue(lua_State* L, int index)
{
	eaPropertyValue::eaTable table;

	if (index < 0)
		--index;

	lua_pushnil(L);
	while (lua_next(L, index))
	{
		eaPropertyValue name = ToPropertyValue(L, -2);
		eaPropertyValue value = ToPropertyValue(L, -1);

		table[name] = value;
		lua_pop(L, 1);
	}

	return table;
}

static eaPropertyValue ToPropertyValue(lua_State* L, int index)
{
	switch (lua_type(L, index))
	{
	case LUA_TNUMBER:
		return make_shared<double>(lua_tonumber(L, index));
	case LUA_TBOOLEAN:
		return make_shared<bool>(lua_toboolean(L, index));
		break;
	case LUA_TSTRING:
		return make_shared<string>(lua_tostring(L, index));
		break;
	case LUA_TTABLE:
		return ToTableValue(L, index);
		break;
	default:
		return nullptr;
		break;
	}
}

/*
获取属性
*/
static int SpritePropertySet(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	sprite->SetProperty(name, ToPropertyValue(L, 3));

	return 0;
}

static void PushPropertyValue(lua_State* L, eaPropertyValue value);

static void PushTable(lua_State* L, eaPropertyValue::eaTable table)
{
	lua_createtable(L, 0, 0);
	for (auto& pair : table)
	{
		PushPropertyValue(L, pair.first);
		PushPropertyValue(L, pair.second);
		lua_settable(L, -3);
	}
}

static void PushPropertyValue(lua_State* L, eaPropertyValue value)
{
	if (value.IsBoolean())
		lua_pushboolean(L, value.ToBoolean());
	else if (value.IsNumber())
		lua_pushnumber(L, value.ToNumber());
	else if (value.IsString())
		lua_pushstring(L, value.ToString().c_str());
	else if (value.IsTable())
		PushTable(L, value.ToTable());
	else
		lua_pushnil(L);
}

/*
设置属性
*/
static int SpritePropertyGet(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	// 尝试获取精灵属性
	auto value = sprite->GetProperty(name);

	if (value != nullptr)
	{
		PushPropertyValue(L, value);

		return 1;
	}

	//尝试获取行为脚本
	auto behaviour = sprite->GetBehaviour(name);
	
	if (behaviour != nullptr && behaviour->IsEnabled())
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, behaviour->GetObjRef());
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

void eaSprite::CreateDomain()
{
	auto& L = eaApplication::GetLua();

	domain = eaLuaDomain::Create("Sprite", scene->GetDomain());

	// 创建sprite对象，控制精灵属性等
	lua_rawgeti(L, LUA_REGISTRYINDEX, domain->GetEnvTableRef());
	lua_pushstring(L, "sprite");
	lua_createtable(L, 0, 0);

	// 创建元表
	lua_createtable(L, 0, 0);
	lua_pushstring(L, "__index");
	lua_pushinteger(L, (long long)this);
	lua_pushcclosure(L, SpritePropertyGet, 1);
	lua_settable(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushinteger(L, (long long)this);
	lua_pushcclosure(L, SpritePropertySet, 1);
	lua_settable(L, -3);
	
	// 设置元表
	lua_setmetatable(L, -2);

	// sprite.name = name
	lua_pushstring(L, "name");
	lua_pushstring(L, name.c_str());
	lua_settable(L, -3);

	// 把sprite对象放入域
	lua_settable(L, -3);
}

eaSpriteBehaviour::eaSpriteBehaviour(eaSprite* sprite, const std::string& name, const std::string& type)
	:eaLuaBridge(sprite->GetDomain(), "behaviour/" + type), name(name)
{
	auto& L = eaApplication::GetLua();

	// behaviour.name = name
	lua_rawgeti(L, LUA_REGISTRYINDEX, domain->GetEnvTableRef());
	lua_pushstring(L, "name");
	lua_pushstring(L, name.c_str());
	lua_settable(L, -3);
}

void eaSpriteBehaviour::Update()
{
	if (objRef == LUA_REFNIL)
		return;

	auto & L = eaApplication::GetLua();

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "update");
	lua_gettable(L, -2);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		cout << "运行脚本" << type << "时出现异常" << endl;
		throw eaLuaError();
	}
}

void eaSpriteBehaviour::Start()
{
	auto& L = eaApplication::GetLua();

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "start");
	lua_gettable(L, -2);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		cout << "启动脚本" << type << "时出现异常" << endl;
		throw eaLuaError();
	}
}

bool eaSpriteBehaviour::IsEnabled()
{
	if (objRef == LUA_REFNIL)
		return false;

	auto & L = eaApplication::GetLua();

	// behaviour.enabled
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "enabled");
	lua_gettable(L, -2);
	bool isEnable = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if (!isEnable)
		Dispose();

	return isEnable;
}