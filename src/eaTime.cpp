#include <SDL.h>
#include "eaTime.h"

eaTime eaTime::time;

void eaTime::Update()
{
	if (eaTime::time.lastUpdateTime == -1)
		eaTime::time.lastUpdateTime = SDL_GetTicks();
	else
		eaTime::time.lastUpdateTime = eaTime::time.currentTime;

	eaTime::time.currentTime = SDL_GetTicks();
}

double eaTime::DeltaTime()
{
	return (eaTime::time.currentTime - eaTime::time.lastUpdateTime) / 1000.0 * eaTime::time.timeScale;
}

double eaTime::NonScaledDeltaTime()
{
	return (eaTime::time.currentTime - eaTime::time.lastUpdateTime) / 1000.0;
}