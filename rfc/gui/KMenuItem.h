
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

#include "../core/CoreModule.h"

class KMenuItemListener;

class KMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	KMenuItemListener* listener;
	KString itemText;
	bool enabled;
	bool checked;
	void* param;
	int intParam;

public:
	KMenuItem();

	virtual void AddToMenu(HMENU hMenu);

	virtual void SetParam(void* param);

	virtual void SetIntParam(int intParam);

	virtual int GetIntParam();

	virtual void* GetParam();

	virtual bool IsChecked();

	virtual void SetCheckedState(bool state);

	virtual bool IsEnabled();

	virtual void SetEnabled(bool state);

	virtual void SetText(const KString& text);

	virtual KString GetText();

	virtual UINT GetItemID();

	virtual HMENU GetMenuHandle();

	virtual void SetListener(KMenuItemListener* listener);

	virtual KMenuItemListener* GetListener();

	virtual void OnPress();

	virtual ~KMenuItem();

private:
	RFC_LEAK_DETECTOR(KMenuItem)
};

