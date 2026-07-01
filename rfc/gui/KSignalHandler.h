
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
#include "KSignal.h"
#include "KComponent.h"
#include <type_traits> // std::is_base_of

// adds onSignal method to KComponent.
// T must be derived from KComponent.
// onSignal executed by the gui thread.
template <class T,
	typename = typename std::enable_if<std::is_base_of<KComponent, T>::value>::type>
class KSignalHandler : public T
{
public:
	template<typename... Args>
	KSignalHandler(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KSignalHandler() noexcept = default;

	virtual void onSignal(WPARAM signalID, LPARAM param) noexcept {}

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override
	{
		if (msg == RFC_SIGNAL_MESSAGE)
		{
			onSignal(wParam, lParam);
			return 0;
		}

		return T::windowProc(hwnd, msg, wParam, lParam);
	}
};