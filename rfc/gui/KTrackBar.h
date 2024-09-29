
/*
	Copyright (C) 2013-2022 CrownSoft
  
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

#include "KComponent.h"

class KTrackBarListener;

class KTrackBar : public KComponent
{
protected:
	int rangeMin,rangeMax,value;
	KTrackBarListener *listener;

public:
	KTrackBar(bool showTicks = false, bool vertical = false);

	/**
		Range between 0 to 100
	*/
	virtual void SetRange(int min, int max);

	virtual void SetValue(int value);

	virtual int GetValue();

	virtual void SetListener(KTrackBarListener* listener);

	virtual void OnChange();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool Create(bool requireInitialMessages = false) override;

	virtual ~KTrackBar();
};



