#include <SDL.h>
#include "eaTexture.h"
#include "eaSpriteImage.h"
#include "eaResources.h"

void eaSpriteImage::Draw(SDL_Renderer* renderer, double groupAlpha)
{
	if (image == nullptr)
		return;

	if (image != nullptr)
	{
		auto renderRect = GetRenderRect();
		auto margin = SDL_Rect{ renderRect.x,renderRect.y,renderRect.width,renderRect.height };
		SDL_SetTextureAlphaMod(*image, (uint8_t)(groupAlpha * alpha * 255 * color.a));
		SDL_SetTextureColorMod(*image, (uint8_t)(255 * color.r), (uint8_t)(255 * color.g), (uint8_t)(255 * color.b));
		SDL_RenderCopy(renderer, *image, nullptr, &margin);
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
	propertyBinder["color"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return { color.r,color.g,color.b,color.a };
		},
		[&](eaPropertyValue value)
		{
			auto table = value.ToTable();

			color.r = table[1];
			color.g = table[2];
			color.b = table[3];
			auto alpha = table.find(4);
			if (alpha != table.end())
				color.a = alpha->second;
			else
				color.a = 1;
		}
	);

	propertyBinder["image"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return image->name;
		},
		[&](eaPropertyValue obj)
		{
			auto name = obj.ToString();
			if (name == "")
				image = nullptr;
			else
				image = eaResources::Load<eaTexture>(name);
		}
	);
}
