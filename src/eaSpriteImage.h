#pragma once
#include "eaSprite.h"

class eaTexture;

class eaSpriteImage : public eaSprite
{
	std::shared_ptr<eaTexture> image;

public:
	eaSpriteImage();

	/*
	绘制
	*/
	void Draw(SDL_Renderer* renderer) override;

	void Save() override;
	void Load() override;
};