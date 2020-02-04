#include <lua.hpp>
#include "eaScriptRunner.h"
#include "eaApplication.h"

using namespace std;

void eaScriptRunner::Update()
{
	auto& L = eaApplication::GetLua();

	if (script == nullptr)
		return;

	if (currentTask != nullptr && !currentTask->IsFinished())
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
		eaApplication::GetLogger().Log("Message", GetStr(block->text.GetRawStr()));
		
	}
	else if (currentBlock.IsType<eaScriptLuaBlock>())
	{
		auto block = currentBlock.Get<eaScriptLuaBlock>();
		GetDomain()->DoString(block->code);
	}

	++currentPos;
}

eaScriptRunner::~eaScriptRunner()
{
	auto& L = eaApplication::GetLua();
}

void eaScriptRunner::Run(std::shared_ptr<eaScript> script)
{
	this->script = script;
	enable = true;
}

void eaScriptRunner::Save()
{

}

void eaScriptRunner::Load()
{
	currentTask->Load();
}

eaScriptTask::eaScriptTask(eaScriptRunner* runner, string name) 
	:eaLuaBridge(runner->GetDomain(), "task/"s + name), runner(runner)
{
}

bool eaScriptTask::IsFinished()
{
	if (objRef == LUA_REFNIL)
		return false;

	auto& L = eaApplication::GetLua();

	// task.enabled
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "finished");
	lua_gettable(L, -2);

	bool isFinished;
	if (lua_isboolean(L, -1))
		isFinished = lua_toboolean(L, -1);
	else
		isFinished = true;
	lua_pop(L, 1);

	return isFinished;
}

void eaScriptTask::Update()
{
	if (objRef == LUA_REFNIL)
		return;

	auto& L = eaApplication::GetLua();

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "update");
	lua_gettable(L, -2);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		eaApplication::GetLogger().Log("LuaError", "刷新任务"s + type + "时出现异常。位置：" + L.GetCurrentInfo());
		throw eaLuaError();
	}
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

		// 单行允许省略return
		if (block->find('\r') == string::npos &&
			block->find("return") == string::npos)
			runner->GetDomain()->DoString("return " + *block);
		else
			runner->GetDomain()->DoString(*block);
	}
	else
		lua_pushnil(L);
}

void eaScriptTask::Start(eaScriptTaskBlock::argList args)
{
	if (objRef == LUA_REFNIL)
		return;

	auto& L = eaApplication::GetLua();

	// task.start
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
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

	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		eaApplication::GetLogger().Log("LuaError", "启动任务"s + type + "时出现异常。位置：" + L.GetCurrentInfo());
		throw eaLuaError();
	}
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

		GetDomain()->DoString(luaCode);
		if (!lua_isnil(L, -1))
			buffer += lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	return buffer;
}

const shared_ptr<eaLuaDomain> eaScriptRunner::GetDomain() const
{
	return scene->GetDomain();
}