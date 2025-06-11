
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

#include "KApplication.h"

HINSTANCE KApplication::hInstance = 0;
KDPIAwareness KApplication::dpiAwareness;
bool KApplication::dpiAwareAPICalled = false;

KApplication::KApplication()
{
}

void KApplication::modifyModuleInitParams()
{

}

int KApplication::main(wchar_t** argv, int argc)
{
	return 0;
}

bool KApplication::allowMultipleInstances()
{
	return true;
}

int KApplication::anotherInstanceIsRunning(wchar_t** argv, int argc)
{
	return 0;
}

const wchar_t* KApplication::getApplicationID()
{
	return L"RFC_APPLICATION";
}

void KApplication::messageLoop(bool handleTabKey)
{
	MSG msg;

	while (::GetMessageW(&msg, NULL, 0, 0))
	{
		if (handleTabKey)
		{
			if (::IsDialogMessage(::GetActiveWindow(), &msg))
				continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
}

KApplication::~KApplication()
{
}
