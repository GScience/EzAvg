#pragma once
#include <memory>

class eaScene;
class eaSprite;

enum MouseButton
{
	MouseButtonLeft = 0,
	MouseButtonRight = 1, 
	MouseButtonMiddle = 2
};

struct MousePoint
{
	int x = 0;
	int y = 0;
};

class eaInput
{
	static bool CheckSpriteInteraction(std::shared_ptr<eaSprite> sprite);
	static void UpdateSceneInteraction(std::shared_ptr<eaScene> scene);

public:
	static MousePoint GetMousePoint();
	static MousePoint GetMousePointDelta();

	static bool GetButtonDown(MouseButton button);
	static bool GetButtonUp(MouseButton button);
	static bool GetButton(MouseButton button);

	static bool GetKeyDown(const std::string& key);
	static bool GetKeyUp(const std::string& key);
	static bool GetKey(const std::string& key);

	static void Update();

	static void Reset();
};