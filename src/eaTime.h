#pragma once

class eaTime
{
	static eaTime time;

	eaTime() = default;

	unsigned int lastUpdateTime = -1;
	unsigned int currentTime = -1;

public:
	double timeScale = 1.0;
	
	static void Update();
	static double DeltaTime();
	static double NonScaledDeltaTime();
};