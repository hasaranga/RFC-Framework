
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

#pragma once

#include "../core/CoreModule.h"

union KInvokeParam {
	int   iParam;
	float fParam;
	void* pParam;
};

// can be used to commnicate between two objects without defining a new interface.
// each receiver reacts only to messages it supports.
// return value semantics depend on the message.
// sender and receiver runs on the same thread. call is synchronous.
template <class SenderPtr = void*>
class KInvokable
{
public:
	virtual int invoke(SenderPtr sender, int msg, KInvokeParam param) noexcept = 0;
};