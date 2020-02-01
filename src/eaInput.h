#pragma once

enum MouseButton
{
	MouseButtonLeft = 0,
	MouseButtonRight = 1, 
	MouseButtonMiddle = 2
};

struct MousePoint
{
	int x;
	int y;
};

class eaInput
{
public:
	static MousePoint GetMousePoint();
	static bool GetButtonDown(MouseButton button);
	static bool GetButtonUp(MouseButton button);
	static bool GetButton(MouseButton button);

	static void Update();
};