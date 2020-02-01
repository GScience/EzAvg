#pragma once
#include <string>
#include <vector>
#include "eaLua.h"
#include "eaScene.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;

class eaApplication : public eaSaveable
{
	void InitWindow();
	void InitApplication();
	void Start();
	void Update();

	std::shared_ptr<eaScene> scene;
	std::shared_ptr<eaLuaDomain> globalDomain;

	eaLua lua;
public:
	static eaApplication* instance;

	SDL_Window* sdlWindow = nullptr;
	SDL_Renderer* sdlRenderer = nullptr;

	eaApplication()
	{
		instance = this;
	}

	/*
	加载场景并卸载当前场景
	*/
	void LoadScene(std::string scriptName)
	{
		scene = eaScene::Load(scriptName);
	}
	
	std::shared_ptr<eaScene> CurrentScene()
	{
		return scene;
	}

	void Save() override;
	void Load() override;

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

	void Run(std::vector<std::string> argv);
};