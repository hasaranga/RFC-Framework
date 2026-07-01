
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
#include <functional>

class KGridView : public KComponent
{
protected:
	int colCount;
	int itemCount;
	virtual void afterCreated() noexcept override;

public:
	std::function<void(KGridView*)> onItemSelect;
	std::function<void(KGridView*)> onItemRightClick;
	std::function<void(KGridView*)> onItemDoubleClick;

	KGridView(bool sortItems = false) noexcept;

	void insertRecord(KString** columnsData) noexcept;

	void insertRecordTo(int rowIndex, KString** columnsData) noexcept;

	KString getRecordAt(int rowIndex, int columnIndex) noexcept;

	/**
		returns -1 if nothing selected.
	*/
	int getSelectedRow() noexcept;

	void removeRecordAt(int rowIndex) noexcept;

	void removeAll() noexcept;

	void updateRecordAt(int rowIndex, int columnIndex, const KString& text) noexcept;

	// columnWidth is a logical value.
	void setColumnWidth(int columnIndex, Logical columnWidth) noexcept;

	// returns a logical value.
	Logical getColumnWidth(int columnIndex) noexcept;

	// columnWidth is a logical value.
	void createColumn(const KString& text, Logical columnWidth = 100) noexcept;

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) noexcept override;

	virtual void _onItemSelect() noexcept;

	virtual void _onItemRightClick() noexcept;

	virtual void _onItemDoubleClick() noexcept;

	virtual ~KGridView() noexcept;
};

