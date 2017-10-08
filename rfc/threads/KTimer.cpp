
/*
    RFC - KTimer.cpp
    Copyright (C) 2013-2017 CrownSoft
  
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

#include "../rfc.h"
#include "../utils/KPlatformUtil.h"
#include "KTimer.h"

KTimer::KTimer()
{
	resolution = 1000;
	started = false;
	listener = 0;
	timerID = KPlatformUtil::GetInstance()->GenerateTimerID(this);
}

void KTimer::SetInterval(int resolution)
{
	this->resolution = resolution;
}

int KTimer::GetInterval()
{
	return resolution;
}

void KTimer::SetTimerWindow(KWindow *window)
{
	this->window = window;
}

void KTimer::SetTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT KTimer::GetTimerID()
{
	return timerID;
}

void KTimer::StartTimer()
{
	if(started)
		return;

	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			::SetTimer(hwnd, timerID, resolution, 0);
			started = true;
		}
	}
}

void KTimer::StopTimer()
{
	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			if(started)
				::KillTimer(hwnd, timerID);

			started = false;
		}
	}
}

bool KTimer::IsTimerRunning()
{
	return started;
}

void KTimer::OnTimer()
{
	if(listener)
		listener->OnTimer(this);
}

void KTimer::SetListener(KTimerListener *listener)
{
	this->listener = listener;
}

KTimer::~KTimer()
{
	if(started)
		this->StopTimer();
}