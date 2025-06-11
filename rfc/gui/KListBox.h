
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

#include "KComponent.h"
#include "../containers/ContainersModule.h"
#include <functional>

class KListBox : public KComponent
{
protected:
	KVector<KString, 10, false> stringList;
	int selectedItemIndex;
	int selectedItemEnd;
	bool multipleSelection;

public:
	std::function<void(KListBox*)> onItemSelect;
	std::function<void(KListBox*)> onItemRightClick;
	std::function<void(KListBox*)> onItemDoubleClick;

	KListBox(bool multipleSelection=false, bool sort=false, bool vscroll=true);

	virtual void addItem(const KString& text);

	virtual void removeItem(int index);

	virtual void removeItem(const KString& text);

	virtual void updateItem(int index, const KString& text);

	virtual int getItemIndex(const KString& text);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual KString getSelectedItem();

	virtual int getSelectedItems(int* itemArray, int itemCountInArray);

	virtual void clearList();

	virtual void selectItem(int index);

	virtual void selectItems(int start, int end);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual void _onItemDoubleClick();

	virtual void _onItemRightClick();

	virtual ~KListBox();
};

