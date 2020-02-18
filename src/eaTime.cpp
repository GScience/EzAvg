#include <SDL.h>
#include "eaTime.h"

eaTime eaTime::time;

void eaTime::Update()
{
	if (time.lastUpdateTime == -1)
		time.lastUpdateTime = SDL_GetTicks();
	else
		time.lastUpdateTime = time.currentTime;

	time.currentTime = SDL_GetTicks();
}

void eaTime::Reset()
{
	time.currentTime = SDL_GetTicks();
	time.lastUpdateTime = time.currentTime;
}

double eaTime::DeltaTime()
{
	return (time.currentTime - time.lastUpdateTime) / 1000.0 * time.timeScale;
}

double eaTime::NonScaledDeltaTime()
{
	return (time.currentTime - time.lastUpdateTime) / 1000.0;
}