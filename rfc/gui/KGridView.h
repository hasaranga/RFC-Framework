
/*
	RFC - KGridView.h
	Copyright (C) 2013-2019 CrownSoft
  
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

#ifndef _RFC_KGRIDVIEW_H_
#define _RFC_KGRIDVIEW_H_

#include "KComponent.h"

class KGridViewListener;

class KGridView : public KComponent
{
protected:
	int colCount;
	int itemCount;
	KGridViewListener *listener;

public:
	KGridView(bool sortItems = false);

	virtual void SetListener(KGridViewListener *listener);

	virtual KGridViewListener* GetListener();

	virtual void InsertRecord(KString **columnsData);

	virtual void InsertRecordTo(int rowIndex, KString **columnsData);

	virtual KString GetRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int GetSelectedRow();

	virtual void RemoveRecordAt(int rowIndex);

	virtual void RemoveAll();

	virtual void UpdateRecordAt(int rowIndex, int columnIndex, const KString& text);

	virtual void SetColumnWidth(int columnIndex, int columnWidth);

	virtual int GetColumnWidth(int columnIndex);

	virtual void CreateColumn(const KString& text, int columnWidth = 100);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual void OnItemSelect();

	virtual void OnItemRightClick();

	virtual void OnItemDoubleClick();

	virtual ~KGridView();
};

#endif