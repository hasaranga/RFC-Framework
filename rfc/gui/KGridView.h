
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
#include <functional>

class KGridView : public KComponent
{
protected:
	int colCount;
	int itemCount;

public:
	std::function<void(KGridView*)> onItemSelect;
	std::function<void(KGridView*)> onItemRightClick;
	std::function<void(KGridView*)> onItemDoubleClick;

	KGridView(bool sortItems = false);

	virtual void insertRecord(KString** columnsData);

	virtual void insertRecordTo(int rowIndex, KString** columnsData);

	virtual KString getRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int getSelectedRow();

	virtual void removeRecordAt(int rowIndex);

	virtual void removeAll();

	virtual void updateRecordAt(int rowIndex, int columnIndex, const KString& text);

	virtual void setColumnWidth(int columnIndex, int columnWidth);

	virtual int getColumnWidth(int columnIndex);

	virtual void createColumn(const KString& text, int columnWidth = 100);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual void _onItemRightClick();

	virtual void _onItemDoubleClick();

	virtual ~KGridView();
};

