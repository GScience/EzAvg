#include <SDL.h>
#include "eaTexture.h"
#include "eaSpriteImage.h"
#include "eaResources.h"

void eaSpriteImage::Draw(SDL_Renderer* renderer)
{
	if (image == nullptr)
		return;

	if (image != nullptr)
	{
		SDL_SetTextureAlphaMod(*image, (uint8_t)(alpha * 255));
		SDL_RenderCopy(renderer, *image, nullptr, nullptr);
	}
}

void eaSpriteImage::Save()
{
}

void eaSpriteImage::Load()
{
}

eaSpriteImage::eaSpriteImage()
{
	propertyBinder["image"] = eaPropertyBinder(
		nullptr,
		[&](eaPropertyValue obj)
		{
			auto name = obj.ToString();
			if (name == "")
				image = nullptr;
			else
				image = eaResources::Load<eaTexture>(name);
		}
	);

	propertyBinder["alpha"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return alpha;
		},
		[&](eaPropertyValue obj)
		{
			alpha = obj.ToNumber();
		}
	);
}
