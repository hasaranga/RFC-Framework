
/*
	Copyright (C) 2013-2026 CrownSoft
  
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

#include "KPasswordBox.h"

KPasswordBox::KPasswordBox(bool readOnly):KTextBox(readOnly)
{
	pwdChar = L'*';
	compDwStyle = compDwStyle | ES_PASSWORD;
}

void KPasswordBox::setPasswordChar(const wchar_t pwdChar)
{
	this->pwdChar = pwdChar;
	if(compHWND)
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		this->repaint();
	}
}

wchar_t KPasswordBox::getPasswordChar()
{
	return pwdChar;
}

bool KPasswordBox::create(bool requireInitialMessages)
{
	if(KTextBox::create(requireInitialMessages))
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		return true;
	}
	return false;
}

KPasswordBox::~KPasswordBox()
{
}