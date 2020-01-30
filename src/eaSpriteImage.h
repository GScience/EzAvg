#pragma once
#include "eaSprite.h"

class eaTexture;

class eaSpriteImage : public eaSprite
{
	std::shared_ptr<eaTexture> image;

public:
	/*
	设置属性
	*/
	void SetProperty(std::string name, eaPropertyValue obj) override;

	/*
	获取属性
	*/
	eaPropertyValue GetProperty(std::string name) override;

	/*
	绘制
	*/
	void Draw(SDL_Renderer* renderer) override;

	/*
	创建
	*/
	void OnCreate() override;

	void Save() override;
	void Load() override;
};