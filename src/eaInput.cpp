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

vector<bool> previousKeyState;
vector<bool> currentKeyState;

MousePoint previousMousePoint;
MousePoint currentMousePoint;

MousePoint eaInput::GetMousePoint()
{
	return currentMousePoint;
}

MousePoint eaInput::GetMousePointDelta()
{
	MousePoint delta;
	delta.x = currentMousePoint.x - previousMousePoint.x;
	delta.y = currentMousePoint.y - previousMousePoint.y;
	return delta;
}

bool eaInput::GetKeyDown(const std::string& key)
{
	auto scanCode = SDL_GetScancodeFromName(key.c_str());
	if (scanCode == SDL_SCANCODE_UNKNOWN
		|| scanCode >= currentKeyState.size()
		|| scanCode >= previousKeyState.size())
		return false;

	return currentKeyState[scanCode] && !previousKeyState[scanCode];
}

bool eaInput::GetKeyUp(const std::string& key)
{
	auto scanCode = SDL_GetScancodeFromName(key.c_str());
	if (scanCode == SDL_SCANCODE_UNKNOWN 
		|| scanCode >= currentKeyState.size()
		|| scanCode >= previousKeyState.size())
		return false;

	return !currentKeyState[scanCode] && previousKeyState[scanCode];
}

bool eaInput::GetKey(const std::string& key)
{
	auto scanCode = SDL_GetScancodeFromName(key.c_str());
	if (scanCode == SDL_SCANCODE_UNKNOWN || scanCode >= currentKeyState.size())
		return false;
	bool result = currentKeyState[scanCode];
	return result;
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

	previousKeyState = currentKeyState;

	previousMousePoint = currentMousePoint;
	
	// 获取当前状态
	currentLeftButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	currentRightButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
	currentMiddleButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE);
	
	int keyCount;
	auto keyStates = SDL_GetKeyboardState(&keyCount);
	currentKeyState.resize(keyCount);
	for (auto i = 0; i < keyCount; ++i)
		currentKeyState[i] = keyStates[i];

	SDL_Rect rect;
	float scale;
	SDL_RenderGetViewport(eaApplication::GetRenderer(), &rect);
	SDL_RenderGetScale(eaApplication::GetRenderer(), &scale, nullptr);
	SDL_GetMouseState(&currentMousePoint.x, &currentMousePoint.y);

	currentMousePoint.x = (int) (currentMousePoint.x / scale) - rect.x;
	currentMousePoint.y = (int) (currentMousePoint.y / scale) - rect.y;

	// 兼容触摸的偏移计算
	if (GetButtonDown(MouseButtonLeft))
		previousMousePoint = currentMousePoint;

	// 刷新精灵交互状态	
	auto currentScene = eaApplication::instance->GetActiveScene();
	if (currentScene != nullptr)
		UpdateSceneInteraction(currentScene);
}

bool eaInput::CheckSpriteInteraction(shared_ptr<eaSprite> sprite)
{
	if (sprite == nullptr || !sprite->enabled || sprite->destroyed)
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