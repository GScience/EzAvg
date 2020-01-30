#include <SDL.h>
#include "eaApplication.h"
#include "eaTexture.h"

eaTexture::eaTexture(SDL_Surface* surface)
{
	texture = SDL_CreateTextureFromSurface(eaApplication::GetRenderer(), surface);
}

eaTexture::~eaTexture()
{
	SDL_DestroyTexture(texture);
}

