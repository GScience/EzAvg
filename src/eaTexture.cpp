#include <SDL.h>
#include <SDL_image.h>
#include "eaApplication.h"
#include "eaTexture.h"

eaTexture::eaTexture(std::string name) :name(name)
{
	auto surface = IMG_Load(("image/" + name).c_str());
	texture = SDL_CreateTextureFromSurface(eaApplication::GetRenderer(), surface);
	SDL_FreeSurface(surface);
	height = surface->h;
	width = surface->w;
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

eaTexture::~eaTexture()
{
	SDL_DestroyTexture(texture);
}

int eaTexture::GetWidth()
{
	return width;
}

int eaTexture::GetHeight()
{
	return height;
}