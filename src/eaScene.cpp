#include <SDL.h>
#include "eaApplication.h"
#include "eaResources.h"
#include "eaScene.h"

void eaScene::Draw(SDL_Renderer* renderer)
{
	for (auto& sprite : sprites)
		sprite->Draw(renderer);
}

void eaScene::Update()
{
	if (runner.enable)
		runner.Update();

	for (auto& sprite : sprites)
		sprite->Update();
}

void eaScene::Save()
{
	for (auto& sprite : sprites)
		sprite->Save();
}

void eaScene::Load()
{
	for (auto& sprite : sprites)
		sprite->Load();
}

void eaScene::RunScript(std::string name)
{
	auto script = eaResources::Load<eaScript>(name);
	runner.Run(script);
}

void eaScene::RemoveSprite(std::string name)
{
	for (auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite)
	{
		if ((*sprite)->name != name)
			continue;

		sprites.erase(sprite);
		return;
	}
}

std::shared_ptr<eaSprite> eaScene::GetSprite(std::string name)
{
	for (auto sprite : sprites)
		if (sprite->name == name)
			return sprite;
	return nullptr;
}
