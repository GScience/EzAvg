#include <SDL.h>
#include "eaApplication.h"
#include "eaInput.h"

using namespace std;

bool previousLeftButtonState = false;
bool previousRightButtonState = false;
bool previousMiddleButtonState = false;

bool currentLeftButtonState = false;
bool currentRightButtonState = false;
bool currentMiddleButtonState = false;

vector<shared_ptr<eaSprite>> previousSelectedSprites;
vector<shared_ptr<eaSprite>> currentSelectedSprites;

MousePoint eaInput::GetMousePoint()
{
	MousePoint point;
	SDL_GetMouseState(&point.x, &point.y);
	return point;
}

bool eaInput::GetButtonDown(MouseButton button)
{
	switch (button)
	{
	case MouseButtonLeft:
		return currentLeftButtonState && !previousLeftButtonState;
	case MouseButtonRight:
		return currentRightButtonState && !previousRightButtonState;
	case MouseButtonMiddle:
		return currentMiddleButtonState && !previousMiddleButtonState;
	default:
		return false;
	}
}

bool eaInput::GetButtonUp(MouseButton button)
{
	switch (button)
	{
	case MouseButtonLeft:
		return !currentLeftButtonState && previousLeftButtonState;
	case MouseButtonRight:
		return !currentRightButtonState && previousRightButtonState;
	case MouseButtonMiddle:
		return !currentMiddleButtonState && previousMiddleButtonState;
	default:
		return false;
	}
}

bool eaInput::GetButton(MouseButton button)
{
	switch (button)
	{
	case MouseButtonLeft:
		return currentLeftButtonState;
	case MouseButtonRight:
		return currentRightButtonState;
	case MouseButtonMiddle:
		return currentMiddleButtonState;
	default:
		return false;
	}
}

void eaInput::Update()
{
	auto mouseState = SDL_GetMouseState(nullptr, nullptr);

	// 记录上一个状态
	previousLeftButtonState = currentLeftButtonState;
	previousRightButtonState = currentRightButtonState;
	previousMiddleButtonState = currentMiddleButtonState;
	previousSelectedSprites = currentSelectedSprites;

	// 获取当前状态
	currentLeftButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	currentRightButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
	currentMiddleButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE);

	// 刷新精灵交互状态	
	auto currentScene = eaApplication::instance->GetActiveScene();
	if (currentScene != nullptr)
		UpdateSceneInteraction(currentScene);
}

bool eaInput::CheckSpriteInteraction(shared_ptr<eaSprite> sprite)
{
	if (!sprite->enabled || sprite->destroyed)
		return false;

	// 判断是否在区域
	auto rect = sprite->GetRenderRect();
	auto mousePos = GetMousePoint();

	if (rect.x > mousePos.x || rect.y > mousePos.y ||
		rect.x + rect.width < mousePos.x || rect.y + rect.height < mousePos.y)
		return false;

	currentSelectedSprites.push_back(sprite);

	// 如果是组，对组内所有对象进行刷新
	if (sprite->GetType() == "group")
	{
		auto spriteGroup = reinterpret_pointer_cast<eaSpriteGroup>(sprite);
		auto sprites = spriteGroup->GetSprites();
		for (auto subSprite = sprites.rbegin(); subSprite != sprites.rend(); ++subSprite)
		{
			if (CheckSpriteInteraction(*subSprite))
				break;
		}
	}

	// 发送消息
	if (GetButtonDown(MouseButtonLeft))
		sprite->SendMessage("onMouseDown");
	else if(GetButtonUp(MouseButtonLeft))
		sprite->SendMessage("onMouseUp");

	return true;
}

void eaInput::UpdateSceneInteraction(shared_ptr<eaScene> scene)
{
	// 获取最新的状态
	currentSelectedSprites.clear();

	CheckSpriteInteraction(scene->GetSpriteGroup());

	// 处理鼠标离开事件
	for (auto i = 0; i < previousSelectedSprites.size(); ++i)
	{
		if (
			i >= currentSelectedSprites.size() || 
			previousSelectedSprites[i] != currentSelectedSprites[i]
			)
			previousSelectedSprites[i]->SendMessage("onMouseLeave");
	}

	// 处理鼠标进入事件
	for (auto i = 0; i < currentSelectedSprites.size(); ++i)
	{
		if (
			i >= previousSelectedSprites.size() ||
			previousSelectedSprites[i] != currentSelectedSprites[i]
			)
			currentSelectedSprites[i]->SendMessage("onMouseEnter");
	}
}

void eaInput::Reset()
{
	previousSelectedSprites.clear();
	currentSelectedSprites.clear();
}