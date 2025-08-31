
/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#include "KTimer.h"
#include "KIDGenerator.h"

KTimer::KTimer()
{
	resolution = 1000;
	started = false;
	timerID = KIDGenerator::getInstance()->generateTimerID(this);
	hwndWindow = NULL;
}

int KTimer::getInterval()
{
	return resolution;
}

void KTimer::setTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT KTimer::getTimerID()
{
	return timerID;
}

void KTimer::start(int resolution, KWindow& window)
{
	if(started)
		return;

	this->resolution = resolution;

	hwndWindow = window.getHWND();
	K_ASSERT(hwndWindow != NULL, "KTimer start called without creating the window!");

	if(hwndWindow)
	{
		::SetTimer(hwndWindow, timerID, resolution, 0);
		started = true;
	}	
}

void KTimer::start(int resolution, KWindow& window, std::function<void(KTimer*)> onTimerCallback)
{
	onTimer = std::move(onTimerCallback);
	start(resolution, window);
}

void KTimer::stop()
{
	if (hwndWindow)
	{
		if(started)
			::KillTimer(hwndWindow, timerID);

		started = false;
	}
}

bool KTimer::isRunning()
{
	return started;
}

void KTimer::_onTimer()
{
	if(onTimer)
		onTimer(this);
}

KTimer::~KTimer()
{
	if(started)
		this->stop();
}