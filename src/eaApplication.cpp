#include <SDL.h>
#include <lua.hpp>
#include "eaApplication.h"
#include "eaTime.h"

eaApplication* eaApplication::instance;

void eaApplication::InitWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	sdlWindow = SDL_CreateWindow(
		"Window",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		500, 500,
		SDL_WINDOW_ALLOW_HIGHDPI);
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
}

void eaApplication::InitApplication()
{
	lua.DoFile("Game.lua");
	auto defaultScene = CreateScene();
}

void eaApplication::Start()
{
	lua.DoString("Game.Start()");
}

void eaApplication::Update()
{
	eaTime::Update();

	auto activeScene = GetActiveScene();
	if (activeScene != nullptr)
		activeScene->Update();
}

void eaApplication::Save()
{
	for (auto& scene : scenes)
		scene->Save();
}

void eaApplication::Load()
{
	for (auto& scene : scenes)
		scene->Load();
}

void eaApplication::Run(std::vector<std::string> argv)
{
	InitWindow();
	InitApplication();
	Start();

	bool shouldWindowClose = false;
	SDL_Event e;

	while (!shouldWindowClose)
	{
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				shouldWindowClose = true;
		}

		Update();

		SDL_RenderClear(sdlRenderer);

		for (auto& scene : scenes)
			scene->Draw(sdlRenderer);

		SDL_RenderPresent(sdlRenderer);
	}

	SDL_Quit();
}
