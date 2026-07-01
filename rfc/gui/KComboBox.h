
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

#include "KComponent.h"
#include "../containers/ContainersModule.h"
#include <functional>

class KComboBox : public KComponent
{
protected:
	KVector<KString, 10, false> stringList;
	int selectedItemIndex;
	virtual void afterCreated() noexcept override;

public:
	std::function<void(KComboBox*)> onItemSelect;

	KComboBox(bool sort=false) noexcept;

	void addItem(const KString& text) noexcept;

	void removeItem(int index) noexcept;

	void removeItem(const KString& text) noexcept;

	int getItemIndex(const KString& text) noexcept;

	int getItemCount() noexcept;

	int getSelectedItemIndex() noexcept;

	KString getSelectedItem() noexcept;

	void clearList() noexcept;

	void selectItem(int index) noexcept;

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) noexcept override;

	virtual void _onItemSelect() noexcept;

	virtual ~KComboBox() noexcept;
};

