#include <SDL.h>
#include <lua.hpp>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "eaApplication.h"
#include "eaTime.h"
#include "eaInput.h"

using namespace std;

eaApplication* eaApplication::instance;

void eaApplication::InitWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	sdlWindow = SDL_CreateWindow(
		"Window",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		applicationSize.width, applicationSize.height,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
	SDL_RenderSetLogicalSize(sdlRenderer, applicationSize.width, applicationSize.height);
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

void eaApplication::SetApplicationSize(int width, int height)
{
	SDL_SetWindowSize(eaApplication::GetWindow(), (int)width, (int)height);
	SDL_SetWindowPosition(eaApplication::GetWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	applicationSize = eaApplicationSize
	{
		width, height
	};
}

void WindowResize(SDL_Window* window)
{

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
			switch (e.type)
			{
				case SDL_QUIT:
				{
					shouldWindowClose = true;
					break;
				}
				case SDL_WINDOWEVENT:
				{
					switch (e.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						{
							SDL_RenderSetLogicalSize(sdlRenderer, applicationSize.width, applicationSize.height);
							break;
						}
					}
				}
			}

		}

		SDL_PumpEvents();
		Update();

		SDL_RenderClear(sdlRenderer);
		scene->Draw(sdlRenderer);
		SDL_RenderPresent(sdlRenderer);
	}

	SDL_Quit();
}