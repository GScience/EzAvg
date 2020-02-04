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
	propertyBinder["rect"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return { rect.up,rect.right,rect.bottom,rect.left };
		},
		[&](eaPropertyValue value)
		{
			int anchorX;
			int anchorY;

			SDL_GetWindowSize(eaApplication::GetWindow(), &anchorX, &anchorY);

			rect.up = value[1];
			rect.right = value[2];
			rect.bottom = value[3];
			rect.left = value[4];
			shared_from_this()->OnResize();
			shared_from_this()->OnMove();
		}
	);
	propertyBinder["anchor"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return
			{
				anchor.minX,anchor.minY,
				anchor.maxX,anchor.maxY
			};
		},
		[&](eaPropertyValue value)
		{
			anchor.minX = value[1];
			anchor.minY = value[2];
			anchor.maxX = value[3];
			anchor.maxY = value[4];
			shared_from_this()->OnResize();
		}
	);
	propertyBinder["pivot"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return
			{
				pivot.x,pivot.y
			};
		},
		[&](eaPropertyValue value)
		{
			pivot.x = value[1];
			pivot.y = value[2];
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
	int windowWidth, windowHeight;
	SDL_GetWindowSize(eaApplication::GetWindow(), &windowWidth, &windowHeight);

	int minX = (int)(anchor.minX * windowWidth);
	int minY = (int)(anchor.minY * windowHeight);
	int maxX = (int)(anchor.maxX * windowWidth);
	int maxY = (int)(anchor.maxY * windowHeight);

	int width = (maxX - minX) - rect.right - rect.left;
	int height = (maxY - minY) - rect.bottom - rect.up;

	int pivotX = (int)((pivot.x - 0.5) * width);
	int pivotY = (int)((pivot.y - 0.5) * height);

	int posX = rect.left;
	int posY = rect.up;

	int x = posX + minX - pivotX;
	int y = posY + minY - pivotY;

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