
/*
	RFC - KTextArea.cpp
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

#include "KTextArea.h"

KTextArea::KTextArea(bool autoScroll, bool readOnly):KTextBox(readOnly)
{
	this->SetSize(200, 100);
	this->SetStyle(compDwStyle | ES_MULTILINE | ES_WANTRETURN);

	if(autoScroll)
		this->SetStyle(compDwStyle | ES_AUTOHSCROLL | ES_AUTOVSCROLL);
	else
		this->SetStyle(compDwStyle | WS_HSCROLL | WS_VSCROLL);
}

bool KTextArea::CreateComponent(bool subClassWindowProc)
{
	return KTextBox::CreateComponent(true); // explicity sublcass windowproc to catch tab key.
}

LRESULT KTextArea::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS; // to catch TAB key
	return KTextBox::WindowProc(hwnd, msg, wParam, lParam);
}

KTextArea::~KTextArea()
{
}