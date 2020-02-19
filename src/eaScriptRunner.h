#pragma once
#include "eaSaveable.h"
#include "eaScript.h"
#include "eaSaveable.h"
#include "eaLuaDomain.h"
#include "eaLuaBridge.h"

class eaScriptRunner;

/*
任务，由Lua控制
*/
class eaScriptTask : public eaLuaBridge
{
	eaScriptTask(eaScriptRunner* runner, std::string name);
	std::shared_ptr<eaLuaDomain> domain;
public:
	const eaScriptRunner* runner;

	eaScriptTask(const eaScriptTask&) = delete;

	bool IsFinished();
	void Update();
	void Start(eaScriptTaskBlock::argList args);

	void Save() override;
	void Load() override;

	static std::shared_ptr<eaScriptTask> Create(eaScriptRunner* runner, std::string name)
	{
		return std::shared_ptr<eaScriptTask>(new eaScriptTask(runner, name));
	}
};

class eaScene;

/*
脚本执行器
*/
class eaScriptRunner : public eaSaveable
{
	friend class eaScene;

	std::shared_ptr<eaScript> script;
	long long currentPos = 0;

	std::shared_ptr<eaScriptTask> currentTask;
	std::shared_ptr<eaScene> scene;

	/*
	是否正在设置文本
	*/
	bool isSettingText = false;
public:
	eaScriptRunner::eaScriptRunner(std::shared_ptr<eaScene> scene) 
		:scene(scene) {}
	eaScriptRunner(const eaScriptRunner&) = delete;
	~eaScriptRunner();

	std::string GetStr(const eaScriptString& scriptStr) const;

	const std::shared_ptr<eaLuaDomain> GetDomain() const;

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

	long long GetNextPos()
	{
		return currentPos;
	}

	/*
	刷新
	*/
	void Update();

	void Save() override;
	void Load() override;
};