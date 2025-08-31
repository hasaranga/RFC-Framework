
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

#pragma once

#include "../core/CoreModule.h"
#include "KWindow.h"
#include <functional>

/**
	Encapsulates a timer.

	The timer can be started with the StartTimer() method
	and controlled with various other methods. Before you start timer, you must set 
	timer window by calling SetTimerWindow method.
*/
class KTimer
{
protected:
	UINT timerID;
	int resolution;
	bool started;
	HWND hwndWindow;

public:
	std::function<void(KTimer*)> onTimer;

	KTimer();

	int getInterval();

	void setTimerID(UINT timerID);

	/**
		@returns unique id of this timer
	*/
	UINT getTimerID();

	// Starts timer. window must be created before calling this method. resolution in ms.
	void start(int resolution, KWindow& window);

	// Starts timer. window must be created before calling this method. resolution in ms.
	void start(int resolution, KWindow& window, std::function<void(KTimer*)> onTimerCallback);

	/**
		Stops the timer. You can restart it by calling start() method.
	*/
	void stop();

	bool isRunning();

	void _onTimer();

	~KTimer();

private:
	RFC_LEAK_DETECTOR(KTimer)
};

