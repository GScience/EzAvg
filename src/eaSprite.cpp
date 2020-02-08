#include <lua.hpp>
#include <SDL.h>
#include "eaSprite.h"
#include "eaApplication.h"

using namespace std;

void eaSprite::Update()
{
	for (auto behaviour = behaviours.begin(); behaviour != behaviours.end();)
	{
		if ((*behaviour)->IsDestroyed())
		{
			behaviour = behaviours.erase(behaviour);
			continue;
		}
		else if ((*behaviour)->IsEnabled())
			(*behaviour)->Update();
		++behaviour;
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
	propertyBinder["name"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return name;
		},
		[&](eaPropertyValue value)
		{
			name = value.ToString();
		}
	);
	propertyBinder["enabled"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return eaPropertyValue(std::make_shared<bool>(enabled));
		},
		[&](eaPropertyValue value)
		{
			enabled = value.ToBoolean();
		}
	);
	propertyBinder["destroyed"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return eaPropertyValue(std::make_shared<bool>(destroyed));
		},
		[&](eaPropertyValue value)
		{
			destroyed = value.ToBoolean();
		}
	);
	propertyBinder["zOrder"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return zOrder;
		},
		[&](eaPropertyValue value)
		{
			zOrder = value;
		}
	);
	propertyBinder["margin"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return { margin.up,margin.right,margin.bottom,margin.left };
		},
		[&](eaPropertyValue value)
		{
			int anchorX;
			int anchorY;

			SDL_GetWindowSize(eaApplication::GetWindow(), &anchorX, &anchorY);

			margin.up = value[1];
			margin.right = value[2];
			margin.bottom = value[3];
			margin.left = value[4];
			shared_from_this()->OnResize();
			shared_from_this()->OnMove();
		}
	);
	propertyBinder["box"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return
			{
				box.x,box.y,
				box.width,box.height
			};
		},
		[&](eaPropertyValue value)
		{
			box.x = value[1];
			box.y = value[2];
			box.width = value[3];
			box.height = value[4];
			shared_from_this()->OnResize();
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

eaRenderRect eaSprite::GetRenderRect()
{
	int x = (int)(box.x + margin.left);
	int y = (int)(box.y + margin.up);

	int width = box.width - margin.right - margin.left;
	int height = box.height - margin.bottom - margin.up;

	return eaRenderRect
	{
		x,y,
		width,height
	};
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

	// 设置自定义对象
	if (sprite->customLuaSetFunction != nullptr)
	{
		int result = sprite->customLuaSetFunction(name);
		if (result != 0)
			return result;
	}

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
创建行为
*/
static int SpriteAddBehaviour(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));

	string behaviourName = lua_tostring(L, 1);
	string behaviourType = lua_tostring(L, 2);

	auto behaviour =  sprite->AddBehaviour(behaviourName, behaviourType);

	if (behaviour == nullptr)
		lua_pushnil(L);
	else
		lua_rawgeti(L, LUA_REGISTRYINDEX, behaviour->GetObjRef());
	
	return 1;
}

/*
设置属性
*/
static int SpritePropertyGet(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	// 方法
	if (name == "addBehaviour")
	{
		lua_pushinteger(L, (long long)sprite);
		lua_pushcclosure(L, SpriteAddBehaviour, 1);
		return 1;
	}

	// 获取自定义对象
	if (sprite->customLuaGetFunction != nullptr)
	{
		int result = sprite->customLuaGetFunction(name);
		if (result != 0)
			return result;
	}

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

void eaSprite::BindDomain(std::shared_ptr<eaLuaDomain> ownerDomain)
{
	auto& L = eaApplication::GetLua();

	domain = eaLuaDomain::Create("Sprite", ownerDomain);

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
		eaApplication::GetLogger().Log("LuaError", "启动行为"s + type + "时出现异常。位置：" + L.GetCurrentInfo());
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
		eaApplication::GetLogger().Log("LuaError", "刷新行为"s + type + "时出现异常。位置：" + L.GetCurrentInfo());
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

	return isEnable;
}


bool eaSpriteBehaviour::IsDestroyed()
{
	if (objRef == LUA_REFNIL)
		return false;

	auto & L = eaApplication::GetLua();

	// behaviour.enabled
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "destroyed");
	lua_gettable(L, -2);
	bool isEnable = lua_toboolean(L, -1);
	lua_pop(L, 1);

	return isEnable;
}