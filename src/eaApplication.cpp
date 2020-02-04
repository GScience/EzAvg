#include <SDL.h>
#include <lua.hpp>
#include <SDL2\SDL_ttf.h>
#include "eaApplication.h"
#include "eaTime.h"
#include "eaInput.h"

using namespace std;

eaApplication* eaApplication::instance;

void eaApplication::InitWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	sdlWindow = SDL_CreateWindow(
		"Window",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		500, 500,
		SDL_WINDOW_ALLOW_HIGHDPI);
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
}

void eaApplication::InitApplication()
{
	globalDomain = eaLuaDomain::Create("Global");
	globalDomain->DoFile("Game.lua");
}

void eaApplication::Start()
{
	globalDomain->DoString("Game.start()");
}

void eaApplication::Update()
{
	eaTime::Update();
	eaInput::Update();
	scene->Update();
	lua.Update();
}

void eaApplication::Save()
{
	scene->Save();
}

void eaApplication::Load()
{
	scene->Load();
}

void eaApplication::Run(std::vector<std::string> args)
{
	bool debugMode = false;
	for (auto& arg : args)
	{
		if (arg == "debug")
			debugMode = true;
	}

	InitWindow();
	InitApplication();
	if (debugMode)
		lua.StartDebuger();
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

		SDL_PumpEvents();
		Update();

		SDL_RenderClear(sdlRenderer);
		scene->Draw(sdlRenderer);
		SDL_RenderPresent(sdlRenderer);
	}

	SDL_Quit();
}