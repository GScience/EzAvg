#pragma once
#include "eaSprite.h"

class eaTexture;

class eaSpriteImage : public eaSprite
{
	std::shared_ptr<eaTexture> image;

public:
	/*
	��������
	*/
	void SetProperty(std::string name, eaPropertyValue obj) override;

	/*
	��ȡ����
	*/
	eaPropertyValue GetProperty(std::string name) override;

	/*
	����
	*/
	void Draw(SDL_Renderer* renderer) override;

	/*
	����
	*/
	void OnCreate() override;

	void Save() override;
	void Load() override;
};