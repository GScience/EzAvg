#include <lua.hpp>
#include <iostream>
#include "easScriptRunner.h"
#include "eaApplication.h"

using namespace std;

void easScriptRunner::Update()
{
	if (script == nullptr)
		return;

	if (currentTask != nullptr && currentTask->IsEnabled())
	{
		currentTask->Update();
		return;
	}

	if (currentPos >= (long long) script->Blocks().size())
	{
		enable = false;
		return;
	}

	auto& currentBlock = script->Blocks()[currentPos];

	if (currentBlock.IsType<easTaskBlock>())
	{
		auto block = currentBlock.Get<easTaskBlock>();
		currentTask = easTask::Create(block->task);
		currentTask->Start(block->args);
	}
	else if (currentBlock.IsType<easTextBlock>())
	{
		auto block = currentBlock.Get<easTextBlock>();
		cout << block->text.GetStr() << endl;
		
	}
	else if (currentBlock.IsType<easLuaBlock>())
	{
		auto block = currentBlock.Get<easLuaBlock>();
		eaApplication::GetLua().DoString(block->code);
	}

	++currentPos;
}

void easScriptRunner::Save()
{
	if (currentTask->IsEnabled())
		currentTask->Save();
}

void easScriptRunner::Load()
{
	currentTask->Load();
}

easTask::easTask(string name)
{
	auto& L = eaApplication::GetLua();
	eaApplication::GetLua().DoFile("task/" + name + ".lua");
 	taskRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

easTask::~easTask()
{
	Dispose();
}

void easTask::Dispose()
{
	if (taskRef == LUA_REFNIL)
		return;

	auto& L = eaApplication::GetLua();

	luaL_unref(L, LUA_REGISTRYINDEX, taskRef);
	taskRef = LUA_REFNIL;
}

bool easTask::IsEnabled()
{
	if (taskRef == LUA_REFNIL)
		return false;

	auto& L = eaApplication::GetLua();

	// task.enabled
	lua_rawgeti(L, LUA_REGISTRYINDEX, taskRef);
	lua_pushstring(L, "enabled");
	lua_gettable(L, -2);
	bool isEnable = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if (!isEnable)
		Dispose();

	return isEnable;
}

void easTask::Update()
{
	if (taskRef == LUA_REFNIL)
		return;

	auto& L = eaApplication::GetLua();
	lua_rawgeti(L, LUA_REGISTRYINDEX, taskRef);
	lua_pushstring(L, "update");
	lua_gettable(L, -2);
	lua_call(L, 0, 0);
}

void easPushObject(eaLua& L, easObject obj)
{
	if (obj.IsType<easEnum>())
		lua_pushstring(L, obj.Get<easEnum>()->c_str());
	else if (obj.IsType<easString>())
		lua_pushstring(L, obj.Get<easString>()->GetStr().c_str());
	else if (obj.IsType<easNumber>())
		lua_pushnumber(L, *obj.Get<easNumber>());
	else if (obj.IsType<easArray>())
	{
		auto& objs = *obj.Get<easArray>();

		lua_createtable(L, 0, 0);

		int index = 1;

		for (auto& obj : objs)
		{
			lua_pushinteger(L, index);
			easPushObject(L, obj);
			lua_settable(L, -3);
			++index;
		}
	}
	else if (obj.IsType<easLuaObject>())
	{
		auto block = obj.Get<easLuaObject>();
		L.DoString(*block);
	}
	else
		lua_pushnil(L);
}

void easTask::Start(easTaskBlock::argList args)
{
	if (taskRef == LUA_REFNIL)
		return;

	auto& L = eaApplication::GetLua();
	
	// task.start
	lua_rawgeti(L, LUA_REGISTRYINDEX, taskRef);
	lua_pushstring(L, "start");
	lua_gettable(L, -2);

	// arg table
	lua_createtable(L, 0, 0);
 	for (auto& arg : args)
	{
		lua_pushstring(L, arg.name.c_str());
		
		easPushObject(L, arg.obj);

		lua_settable(L, -3); 
	}

	lua_call(L, 1, 0);
}

void easTask::Save()
{
}

void easTask::Load()
{
}

string easString::GetStr() const
{
	auto& L = eaApplication::GetLua();
	string buffer;

	for (size_t i = 0; i < str.size(); ++i)
	{
		char c = str[i];

		if (c != '{')
		{
			buffer += c;
			continue;
		}

		string luaCode;
		++i;
		for (; i < str.size(); ++i)
		{
			c = str[i];
			if (c == '}')
				break;

			luaCode += c;
		}

		L.DoString(luaCode);
		if (!lua_isnil(L, -1))
			buffer += lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	return buffer;
}