
/*
	Copyright (C) 2013-2024 CrownSoft
  
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

#include "KStringHolder.h"


KStringHolder::KStringHolder(wchar_t *w_text, int count)
{
	refCount = 1;
	this->w_text = w_text;
	this->count = count;
}

KStringHolder::~KStringHolder()
{

}

void KStringHolder::AddReference()
{
	::InterlockedIncrement(&refCount);
}

void KStringHolder::ReleaseReference()
{
	const LONG res = ::InterlockedDecrement(&refCount);
	if(res == 0)
	{
		if(w_text)
			::free(w_text);

		delete this;
	}
}