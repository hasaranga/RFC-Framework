
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
#include <functional>

class KMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	KString itemText;
	bool enabled;
	bool checked;
	void* param;
	int intParam;

public:
	std::function<void(KMenuItem*)> onPress;

	KMenuItem();

	virtual void addToMenu(HMENU hMenu);

	virtual void setParam(void* param);

	virtual void setIntParam(int intParam);

	virtual int getIntParam();

	virtual void* getParam();

	virtual bool isChecked();

	virtual void setCheckedState(bool state);

	virtual bool isEnabled();

	virtual void setEnabled(bool state);

	virtual void setText(const KString& text);

	virtual KString getText();

	virtual UINT getItemID();

	virtual HMENU getMenuHandle();

	virtual void _onPress();

	virtual ~KMenuItem();

private:
	RFC_LEAK_DETECTOR(KMenuItem)
};

