#include <SDL.h>
#include "eaInput.h"

bool previousLeftButtonState = false;
bool previousRightButtonState = false;
bool previousMiddleButtonState = false;

bool currentLeftButtonState = false;
bool currentRightButtonState = false;
bool currentMiddleButtonState = false;


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

	currentLeftButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	currentRightButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
	currentMiddleButtonState = mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE);
}