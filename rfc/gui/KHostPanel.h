
/*
	Copyright (C) 2013-2023 CrownSoft

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
#include "../containers/ContainersModule.h"

class KHostPanel : public KComponent
{
protected:
	bool enableDPIUnawareMode;
	KPointerList<KComponent*>* componentList;

public:
	KHostPanel();

	// called by the parent
	virtual void SetComponentList(KPointerList<KComponent*>* componentList);

	// called by the parent
	virtual void SetEnableDPIUnawareMode(bool enable);

	/**
		add KHostPanel to window(call create) before adding items to it.
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool AddComponent(KComponent* component, bool requireInitialMessages = false);

	// Can be also use to remove a container. Also destroys the hwnd.
	virtual void RemoveComponent(KComponent* component);

	/**
		add KHostPanel to window(call create) before adding items to it.
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool AddContainer(KHostPanel* container, bool requireInitialMessages = false);

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KHostPanel();
};
