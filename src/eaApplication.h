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

	/*
	支持同时加载多个场景，但是只能有一个场景在刷新
	*/
	std::vector<std::shared_ptr<eaScene>> scenes;

public:
	static eaApplication* instance;

	SDL_Window* sdlWindow = nullptr;
	SDL_Renderer* sdlRenderer = nullptr;

	eaApplication()
	{
		instance = this;
	}

	std::shared_ptr<eaScene> GetActiveScene()
	{
		if (scenes.size() == 0)
			return nullptr;
		auto index = scenes.size() - 1;
		return scenes[index];
	}

	std::shared_ptr<eaScene> CreateScene()
	{
		auto ptr = std::make_shared<eaScene>();
		scenes.push_back(ptr);
		return ptr;
	}

	void RemoveActiveScene()
	{
		if (scenes.size() == 1)
			return;

		scenes.pop_back();
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

	eaLua lua;

	void Run(std::vector<std::string> argv);
};