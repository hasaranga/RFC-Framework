
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

#include "KButton.h"
#include "KMenu.h"

class KMenuButton : public KButton
{
protected:
	KFont *arrowFont;
	KFont *glyphFont;
	KMenu *buttonMenu;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KMenuButton();

	virtual ~KMenuButton();

	virtual void SetMenu(KMenu *buttonMenu);

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual void SetDPI(int newDPI);

	virtual void OnPress();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);
};

