#include <SDL.h>
#include <lua.hpp>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <thread>
#include "eaProfileNode.h"
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
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
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
	if (!isActive)
	{
		eaTime::Reset();
		eaInput::Reset();
		this_thread::sleep_for(chrono::milliseconds(50));
		return;
	}

	eaTime::Update();
	eaInput::Update();
	if (scene != nullptr)
		scene->Update();
	lua.Update();
}

void eaApplication::Draw()
{
	if (!isActive)
		return;

	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);

	SDL_RenderClear(sdlRenderer);
	if (scene != nullptr)
		scene->Draw(sdlRenderer);
	SDL_RenderPresent(sdlRenderer);
}

void eaApplication::SetApplicationSize(int width, int height)
{
	SDL_SetWindowSize(eaApplication::GetWindow(), (int)width, (int)height);
	SDL_SetWindowPosition(eaApplication::GetWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	SDL_RenderSetLogicalSize(sdlRenderer, width, height);

	if (scene != nullptr)
	{
		eaSpriteBox box;
		
		box.x = 0;
		box.y = 0;
		box.width = width;
		box.height = height;

		scene->GetSpriteGroup()->box = box;
		scene->GetSpriteGroup()->OnLayoutChanged();
	}

	applicationSize = eaApplicationSize
	{
		width, height
	};
}

void eaApplication::LoadScene(std::string scriptName)
{
	eaInput::Reset();
	scene = eaScene::Load(scriptName);
}

void eaApplication::LoadProfile(std::string profileName)
{
	eaProfileNode saveNode;

	auto sceneNode = saveNode.Get<eaProfileNode>("Scene");
	auto sceneName = sceneNode->Get<eaPropertyValue>("Name");
	LoadScene(*sceneName);
	scene->Load(sceneNode);
}

void eaApplication::SaveProfile(std::string profileName)
{
	eaProfileNode saveNode;

	auto sceneNode = saveNode.Set("Scene");
	auto _ = sceneNode->Set("Name", scene->name);
	scene->Save(sceneNode);
}

string eaApplication::GetProfileInfo(std::string profileName)
{
	return "";
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
	
	while (!shouldWindowClose && !closed)
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
						case SDL_WINDOWEVENT_FOCUS_LOST:
						{
							isActive = false;
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED:
						{
							isActive = true;
							break;
						}
						case SDL_WINDOWEVENT_MOVED:
						{
							eaTime::Reset();
							eaInput::Reset();
							break;
						}
						case SDL_WINDOWEVENT_RESIZED:
						{
							eaTime::Reset();
							eaInput::Reset();
							break;
						}
					}
				}
			}

		}

		SDL_PumpEvents();
		Update();
		Draw();
	}

	SDL_Quit();
}