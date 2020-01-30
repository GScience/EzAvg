#include <SDL.h>
#include "eaTexture.h"
#include "eaSpriteImage.h"
#include "eaResources.h"

void eaSpriteImage::SetProperty(std::string name, eaPropertyValue obj)
{
	if (name == "image")
	{
		if (!obj.IsString())
			return;

		if (obj.ToString() == "")
			image = nullptr;
		else
			image = eaResources::Load<eaTexture>(obj.ToString());
	}
}

eaPropertyValue eaSpriteImage::GetProperty(std::string name)
{
	return eaPropertyValue(nullptr);
}

void eaSpriteImage::Draw(SDL_Renderer* renderer)
{
	if (image == nullptr)
		return;

	SDL_RenderCopy(renderer, *image, nullptr, nullptr);
}

void eaSpriteImage::OnCreate()
{
}

void eaSpriteImage::Save()
{
}

void eaSpriteImage::Load()
{
}
