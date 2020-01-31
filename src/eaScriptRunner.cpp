#include <lua.hpp>
#include <iostream>
#include "eaScriptRunner.h"
#include "eaApplication.h"

using namespace std;

void eaScriptRunner::Update()
{
	auto& L = eaApplication::GetLua();

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

	if (currentBlock.IsType<eaScriptTaskBlock>())
	{
		auto block = currentBlock.Get<eaScriptTaskBlock>();
		currentTask = eaScriptTask::Create(this, block->task);
		currentTask->Start(block->args);
	}
	else if (currentBlock.IsType<eaScriptTextBlock>())
	{
		auto block = currentBlock.Get<eaScriptTextBlock>();
		cout << GetStr(block->text.GetRawStr()) << endl;
		
	}
	else if (currentBlock.IsType<eaScriptLuaBlock>())
	{
		auto block = currentBlock.Get<eaScriptLuaBlock>();
		luaEnv->DoString(L, block->code);
	}

	++currentPos;
}

eaScriptRunner::~eaScriptRunner()
{
	auto& L = eaApplication::GetLua();
}

void eaScriptRunner::Run(std::shared_ptr<eaScript> script)
{
	auto& L = eaApplication::GetLua();

	this->script = script;
	luaEnv = std::make_shared<eaLuaEnv>(L, "123");
	enable = true;
}

void eaScriptRunner::Save()
{
	if (currentTask->IsEnabled())
		currentTask->Save();
}

void eaScriptRunner::Load()
{
	currentTask->Load();
}

eaScriptTask::eaScriptTask(eaScriptRunner* runner, string name) :runner(runner)
{
	auto& L = eaApplication::GetLua();
	eaApplication::GetLua().DoFile("task/" + name + ".lua");
 	taskRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

eaScriptTask::~eaScriptTask()
{
	Dispose();
}

void eaScriptTask::Dispose()
{
	if (taskRef == LUA_REFNIL)
		return;

	auto& L = eaApplication::GetLua();

	luaL_unref(L, LUA_REGISTRYINDEX, taskRef);
	taskRef = LUA_REFNIL;
}

bool eaScriptTask::IsEnabled()
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

void eaScriptTask::Update()
{
	if (taskRef == LUA_REFNIL)
		return;

	auto& L = eaApplication::GetLua();
	lua_rawgeti(L, LUA_REGISTRYINDEX, taskRef);
	lua_pushstring(L, "update");
	lua_gettable(L, -2);
	lua_call(L, 0, 0);
}

void easPushObject(eaLua& L, const eaScriptRunner* runner, eaScriptObject obj)
{
	if (obj.IsType<eaScriptEnum>())
		lua_pushstring(L, obj.Get<eaScriptEnum>()->c_str());
	else if (obj.IsType<eaScriptString>())
		lua_pushstring(L, runner->GetStr(obj.Get<eaScriptString>()->GetRawStr()).c_str());
	else if (obj.IsType<eaScriptNumber>())
		lua_pushnumber(L, *obj.Get<eaScriptNumber>());
	else if (obj.IsType<eaScriptArray>())
	{
		auto& objs = *obj.Get<eaScriptArray>();

		lua_createtable(L, 0, 0);
		
		int index = 1;

		for (auto& obj : objs)
		{
			lua_pushinteger(L, index);
			easPushObject(L, runner, obj);
			lua_settable(L, -3);
			++index;
		}
	}
	else if (obj.IsType<eaScriptLuaObject>())
	{
		auto block = obj.Get<eaScriptLuaObject>();
		runner->GetEnv()->DoString(L, *block);
	}
	else
		lua_pushnil(L);
}

void eaScriptTask::Start(eaScriptTaskBlock::argList args)
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
		
		easPushObject(L, runner, arg.obj);

		lua_settable(L, -3); 
	}

	lua_call(L, 1, 0);
}

void eaScriptTask::Save()
{
}

void eaScriptTask::Load()
{
}

std::string eaScriptRunner::GetStr(const eaScriptString& scriptStr) const
{
	auto& L = eaApplication::GetLua();
	auto& str = scriptStr.GetRawStr();

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

		luaEnv->DoString(L, luaCode);
		if (!lua_isnil(L, -1))
			buffer += lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	return buffer;
}