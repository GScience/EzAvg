#pragma once
#include "eaSaveable.h"
#include "easScript.h"
#include "eaSaveable.h"

class easTask : public eaSaveable
{
	int taskRef;

	easTask(std::string name);
public:
	easTask(const easTask&) = delete;
	~easTask();

	void Dispose();
	bool IsEnabled();
	void Update();
	void Start(easTaskBlock::argList args);

	void Save() override;
	void Load() override;

	static std::shared_ptr<easTask> Create(std::string name)
	{
		return std::shared_ptr<easTask>(new easTask(name));
	}
};

/*
脚本执行器
*/
class easScriptRunner : public eaSaveable
{
	std::shared_ptr<easScript> script;
	long long currentPos = 0;

	std::shared_ptr<easTask> currentTask;

public:
	easScriptRunner() = default;
	easScriptRunner(const easScriptRunner&) = delete;

	/*
	脚本是否激活
	*/
	bool enable = false;

	void Run(std::shared_ptr<easScript> script)
	{
		this->script = script;
		enable = true;
	}

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