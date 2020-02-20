#pragma once
#include <string>
#include <vector>
#include "eaLogger.h"
#include "eaLua.h"
#include "eaScene.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;

struct eaApplicationSize
{
	int width = 500;
	int height = 500;
};

class eaApplication : public eaSaveable
{
	std::shared_ptr<eaScene> scene;
	std::shared_ptr<eaLuaDomain> globalDomain;

	eaLua lua;
	eaLogger logger;

public:
	bool closed = false;
	bool isActive = true;

	static eaApplication* instance;

	SDL_Window* sdlWindow = nullptr;
	SDL_Renderer* sdlRenderer = nullptr;

	eaApplicationSize applicationSize;

	eaApplication()
	{
		instance = this;
	}

private:
	void InitWindow();
	void InitApplication();
	void Start();
	void Update();
	void Draw();

public:
	/*
	加载场景并卸载当前场景
	*/
	void LoadScene(std::string scriptName);
	
	/*
	设置窗体大小
	*/
	void SetApplicationSize(int width, int height);

	std::shared_ptr<eaScene> CurrentScene()
	{
		auto currentScene = scene;
		// 获取最深层
		while (currentScene->GetPopScene() != nullptr)
			currentScene = currentScene->GetPopScene();
		return currentScene;
	}

	void Save() override;
	void Load() override;

	static eaLogger& GetLogger()
	{
		return instance->logger;
	}

	static SDL_Window* GetWindow()
	{
		return instance->sdlWindow;
	}

	static SDL_Renderer* GetRenderer()
	{
		return instance->sdlRenderer;
	}

	static eaLua& GetLua()
	{
		return instance->lua;
	}

	static std::shared_ptr<eaLuaDomain> GetDomain()
	{
		return instance->globalDomain;
	}

	void Run(std::vector<std::string> args);
};