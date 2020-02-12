#include <lua.hpp>
#include <algorithm>
#include "eaApplication.h"
#include "eaSpriteGroup.h"
#include "eaSpriteImage.h"
#include "eaSpriteText.h"

using namespace std;

static int SpriteGroupAddSprite(lua_State* L)
{
	auto spriteGroup = (eaSpriteGroup*)lua_tointeger(L, lua_upvalueindex(1));

	string spriteName = lua_tostring(L, 1);
	string spriteType = lua_tostring(L, 2);

	std::shared_ptr<eaSprite> sprite;

	if (spriteType == "image")
		sprite = spriteGroup->AddSprite<eaSpriteImage>(spriteName);
	else if (spriteType == "text")
		sprite = spriteGroup->AddSprite<eaSpriteText>(spriteName);
	else if (spriteType == "group")
		sprite = spriteGroup->AddSprite<eaSpriteGroup>(spriteName);

	if (sprite == nullptr)
		lua_pushnil(L);
	else
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetDomain()->GetEnvTableRef());
		lua_pushstring(L, "sprite");
		lua_gettable(L, -2);
	}
	return 1;
}

static int SpriteGroupSpriteIterator(lua_State* L)
{
	auto spriteGroup = (eaSpriteGroup*)lua_tointeger(L, lua_upvalueindex(1));

	size_t currentNumber = 0;

	if (lua_isinteger(L, 2))
		currentNumber = (size_t)lua_tointeger(L, 2);

	auto sprites = spriteGroup->GetSprites();

	++currentNumber;

	if (currentNumber > sprites.size())
		return 0;

	auto sprite = sprites[currentNumber - 1];

	lua_pushinteger(L, currentNumber);
	lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetDomain()->GetEnvTableRef());
	lua_pushstring(L, "sprite");
	lua_gettable(L, -2);
	lua_replace(L, -2);

	return 2;
}

eaSpriteGroup::eaSpriteGroup()
{
	auto& L = eaApplication::GetLua();

	customLuaGetFunctions.push_back([&](string name)
	{
		// SpriteGroup方法
		if (name == "addSprite")
		{
			lua_pushinteger(L, (long long)this);
			lua_pushcclosure(L, SpriteGroupAddSprite, 1);
			return 1;
		}
		else if (name == "iter")
		{
			lua_pushinteger(L, (long long)this);
			lua_pushcclosure(L, SpriteGroupSpriteIterator, 1);
			return 1;
		}

		// SpriteGroup子精灵索引
		auto sprite = GetSprite(name);
		if (sprite == nullptr)
			return 0;

		lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetDomain()->GetEnvTableRef());
		lua_pushstring(L, "sprite");
		lua_gettable(L, -2);

		return 1;
	});
}

void eaSpriteGroup::Draw(SDL_Renderer* renderer)
{
	sort(sprites.begin(), sprites.end(), [](std::shared_ptr<eaSprite> a, std::shared_ptr<eaSprite> b)
		{
			return (a->GetZOrder() < b->GetZOrder());
		});

	for (auto& sprite : sprites)
	{
		if (sprite->enabled && !sprite->destroyed)
			sprite->Draw(renderer);
	}
}

void eaSpriteGroup::Update()
{
	eaSprite::Update();

	for (auto sprite = sprites.begin(); sprite != sprites.end();)
	{
		if ((*sprite)->destroyed)
		{
			sprite = sprites.erase(sprite);
			continue;
		}
		else if ((*sprite)->enabled)
		{
			(*sprite)->Update();
		}
		++sprite;
	}
}

void eaSpriteGroup::Save()
{
}

void eaSpriteGroup::Load()
{
}

shared_ptr<eaSprite> eaSpriteGroup::GetSprite(string name)
{
	for (auto sprite : sprites)
		if (sprite->name == name)
			return sprite;
	return nullptr;
}