#pragma once

struct SDL_Texture;
struct SDL_Surface;

class eaTexture
{
	SDL_Texture* texture;
public:
	eaTexture(SDL_Surface* surface);
	~eaTexture();

	operator SDL_Texture* ()
	{
		return texture;
	}
};