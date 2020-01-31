#pragma once
#include "eaSaveable.h"
#include "eaScript.h"
#include "eaSaveable.h"
#include "eaLuaEnv.h"

class eaScriptRunner;

class eaScriptTask : public eaSaveable
{
	int taskRef;

	eaScriptTask(eaScriptRunner* runner, std::string name);
public:
	const eaScriptRunner* runner;

	eaScriptTask(const eaScriptTask&) = delete;
	~eaScriptTask();

	void Dispose();
	bool IsEnabled();
	void Update();
	void Start(eaScriptTaskBlock::argList args);

	void Save() override;
	void Load() override;

	static std::shared_ptr<eaScriptTask> Create(eaScriptRunner* runner, std::string name)
	{
		return std::shared_ptr<eaScriptTask>(new eaScriptTask(runner, name));
	}
};

/*
脚本执行器
*/
class eaScriptRunner : public eaSaveable
{
	std::shared_ptr<eaScript> script;
	long long currentPos = 0;

	std::shared_ptr<eaScriptTask> currentTask;
	std::shared_ptr<eaLuaEnv> luaEnv;

public:
	eaScriptRunner() = default;
	eaScriptRunner(const eaScriptRunner&) = delete;
	~eaScriptRunner();

	std::string GetStr(const eaScriptString& scriptStr) const;

	const std::shared_ptr<eaLuaEnv> GetEnv() const
	{
		return luaEnv;
	}

	/*
	脚本是否激活
	*/
	bool enable = false;

	void Run(std::shared_ptr<eaScript> script);

	bool Jump(long long pos)
	{
		if (pos < 0 || pos >= (long long) script->Blocks().size())
			return false;

		if (currentTask != nullptr)
			currentTask->Dispose();

		currentPos = pos;

		return true;
	}

	bool Jump(std::string label)
	{
		auto pos = script->GetLabelPos(label);
		return Jump(pos);
	}

	/*
	刷新
	*/
	void Update();

	void Save() override;
	void Load() override;
};