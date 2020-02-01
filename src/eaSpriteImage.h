#pragma once
#include "eaSprite.h"

class eaTexture;

class eaSpriteImage : public eaSprite
{
	std::shared_ptr<eaTexture> image;

	double alpha = 1;

public:
	eaSpriteImage();

	/*
	ªÊ÷∆
	*/
	void Draw(SDL_Renderer* renderer) override;

	/*
	¥¥Ω®
	*/
	void OnCreate() override;

	void Save() override;
	void Load() override;
};