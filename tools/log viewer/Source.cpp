
#include "rfc_amalgamated.h"

#define COL_WARNING_TEXT RGB(0, 128, 0)
#define COL_WARNING_BACK RGB(240, 240, 240)
#define COL_ERROR_TEXT RGB(149, 0, 0)
#define COL_ERROR_BACK RGB(255, 240, 240)

/**
	custom drawn gridview to highlight warnings & errors.
	for some components, custom drawing is easier than owner drawing. 
	see: https://www.codeproject.com/Articles/79/Neat-Stuff-to-Do-in-List-Controls-Using-Custom-Dra
*/
class KLogViewingGrid : public KGridView
{
public:

	bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
	{
		if (msg == WM_NOTIFY)
		{		
			if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
			{
				LPNMLVCUSTOMDRAW pLVCD = (LPNMLVCUSTOMDRAW) lParam;

				*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

				if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage) // it's the control's prepaint stage, tell Windows we want messages for every item.
				{
					*result = CDRF_NOTIFYITEMDRAW;
				}
				else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage ) //  prepaint stage for an item. (subitems will use same values.)
				{
					if (pLVCD->nmcd.lItemlParam == KLogger::EVT_ERROR)
					{
						pLVCD->clrTextBk = COL_ERROR_BACK;
						pLVCD->clrText = COL_ERROR_TEXT;
					}
					else if (pLVCD->nmcd.lItemlParam == KLogger::EVT_WARNING)
					{
						pLVCD->clrTextBk = COL_WARNING_BACK;
						pLVCD->clrText = COL_WARNING_TEXT;
					}
				
					*result = CDRF_DODEFAULT; // tell Windows to paint the control itself.
				}

				return true; // indicate that we processed this msg & result is valid.
			}
		}

		return KGridView::EventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
	}

	void InsertRecord(KString **columnsData, int eventType)
	{
		LVITEMW lvi = { 0 };
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[0]);
		lvi.iItem = itemCount;
		lvi.lParam = (LPARAM)eventType;

		int row = (int)::SendMessageW(compHWND, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

		for (int i = 1; i < colCount; i++) // first column already added, lets add the others
		{
			LV_ITEMW lvi = { 0 };
			lvi.iSubItem = i;
			lvi.pszText = (wchar_t*)(const wchar_t*)(*columnsData[i]);

			::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvi);
		}

		itemCount++;
	}

	/**
		never used, but it's just here to show that we are using LPARAM to store event type.
	*/
	int GetEventTypeOfRecord(int rowIndex)
	{
		LV_ITEMW lvi = { 0 };
		lvi.iItem = rowIndex;
		lvi.mask = LVIF_PARAM;

		ListView_GetItem(compHWND, &lvi);

		return (int)lvi.lParam;
	}
};

class MainWindow : public KOverlappedWindow, public KGridViewListener, public KMenuItemListener, public KThread
{
protected:
	KLogViewingGrid gridView;
	KMenuItem miOpenFile, miExit;
	KMenu fileMenu;
	KMenuBar menuBar;

	KString logFilePath;

public:
	MainWindow()
	{
		this->SetSize(800, 600);
		this->SetText(STATIC_TXT("Log Viewer"));
		this->CreateComponent();
		this->CenterScreen();

		// create menu
		miOpenFile.SetText(STATIC_TXT("Open Log File..."));
		miExit.SetText(STATIC_TXT("Exit"));
	
		miOpenFile.SetListener(this);
		miExit.SetListener(this);

		fileMenu.AddMenuItem(&miOpenFile);
		fileMenu.AddSeperator();
		fileMenu.AddMenuItem(&miExit);
		
		menuBar.AddMenu(STATIC_TXT("File"),&fileMenu);
		menuBar.AddToWindow(this);

		this->AddComponent(&gridView);

		gridView.SetListener(this);

		int clientWidth, clientHeight;
		this->GetClientAreaSize(&clientWidth, &clientHeight);

		gridView.SetSize(clientWidth, clientHeight);		
		gridView.CreateColumn(STATIC_TXT("Time (sec : mills)"));
		gridView.CreateColumn(STATIC_TXT("Type"));
		gridView.CreateColumn(STATIC_TXT("Details"), clientWidth - (gridView.GetColumnWidth(0) + gridView.GetColumnWidth(1) + 40));	
	}

	void OnMenuItemPress(KMenuItem *menuItem)
	{
		if (menuItem == &miOpenFile)
		{			
			if (KCommonDialogBox::ShowOpenFileDialog(this, L"Open Log File...", L"Log Files (*.rlog)\0*.rlog\0", &logFilePath))
			{
				gridView.RemoveAll();
				this->SetText(STATIC_TXT("Log Viewer"));
				StartThread();
			}
		}
		else if (menuItem == &miExit)
		{
			this->ThreadShouldStop();
			this->WaitUntilThreadFinish();

			this->OnClose();
		}
	}

	void Run()
	{
		KFile logFile;

		if (logFile.OpenFile(logFilePath, KFile::KREAD))
		{
			char header[4];
			logFile.ReadFile(header, 4);

			if ((header[0] = 'R') && (header[1] = 'L') && (header[2] = 'O') && (header[3] = 'G'))
			{
				unsigned int totalEvents = 0;
				logFile.ReadFile(&totalEvents, sizeof(unsigned int));

				int errorEventsCount = 0;
				int warningEventsCount = 0;

				for (unsigned int i = 0; i < totalEvents; i++)
				{
					unsigned char eventType;
					logFile.ReadFile(&eventType, 1);

					if (eventType == KLogger::EVT_ERROR)
						errorEventsCount++;
					else if (eventType == KLogger::EVT_WARNING)
						warningEventsCount++;

					unsigned short secs;
					logFile.ReadFile(&secs, sizeof(unsigned short));

					unsigned short mills;
					logFile.ReadFile(&mills, sizeof(unsigned short));

					KString details;
					while (true) // iterate through each params
					{
						unsigned char paramType;
						logFile.ReadFile(&paramType, 1);

						if (paramType == KLogger::PARAM_STRING)
						{
							unsigned char dataSize;
							logFile.ReadFile(&dataSize, 1);

							char buff[256];
							logFile.ReadFile(buff, dataSize);
							buff[dataSize] = 0;

							details = details + KString(buff);
						}
						else if (paramType == KLogger::PARAM_INT32)
						{
							int data = 0;
							logFile.ReadFile(&data, sizeof(int));

							details = details + KString(data);
						}
						else if (paramType == KLogger::PARAM_SHORT16)
						{
							unsigned short data = 0;
							logFile.ReadFile(&data, sizeof(unsigned short));

							details = details + KString((int)data);
						}
						else if (paramType == KLogger::PARAM_FLOAT)
						{
							float data = 0;
							logFile.ReadFile(&data, sizeof(float));

							details = details + KString(data, 4, true);
						}
						else if (paramType == KLogger::PARAM_DOUBLE)
						{
							double data = 0;
							logFile.ReadFile(&data, sizeof(double));

							details = details + KString((float)data, 4, true);
						}
						else if (paramType == KLogger::EVT_END)
						{
							break;
						}
						else
						{
							MessageBoxW(this->GetHWND(), L"Invalid param type defined!", L"Error", MB_ICONERROR);
							goto DONE;
						}
					}

					char strMills[5];
					sprintf(strMills, "%03d", (int)mills); // zero padded mills
					
					AddRecord(KString((int)secs) + STATIC_TXT(" : ") + KString(strMills, KString::USE_COPY_OF_TEXT), eventType, details);

					if (threadShouldStop)
						break;
				}
				this->SetText(STATIC_TXT("Log Viewer - ") + KString(totalEvents) + STATIC_TXT(" Events , ") + KString(errorEventsCount) + STATIC_TXT(" Errors , ") + KString(warningEventsCount) + STATIC_TXT(" Warnings"));
			}
			else{
				MessageBoxW(this->GetHWND(), L"Invalid log file!", L"Error", MB_ICONERROR);
			}
		}
		else{
			MessageBoxW(this->GetHWND(), L"Cannot read log file!", L"Error", MB_ICONERROR);
		}

DONE:
		isThreadRunning = false;
	}

	void AddRecord(KString time, unsigned char eventType, KString details)
	{
		const wchar_t *eventTypes[3] = { L"Information", L"Warning", L"Error" };
		KString eventStr(eventTypes[eventType - 1], KString::STATIC_TEXT_DO_NOT_FREE);

		KString* items[3];
		items[0] = &time;
		items[1] = &eventStr;
		items[2] = &details;

		gridView.InsertRecord(items, eventType);
	}

	void OnGridViewItemRightClick(KGridView *gridView)
	{
		int row = gridView->GetSelectedRow();
		if (row != -1)
		{
			// you can show popup menu in here...
		}
	}

	void OnResized()
	{
		int clientWidth, clientHeight;
		this->GetClientAreaSize(&clientWidth, &clientHeight);

		gridView.SetSize(clientWidth, clientHeight);
		gridView.SetColumnWidth(0, 100);
		gridView.SetColumnWidth(1, 100);
		gridView.SetColumnWidth(2, clientWidth - (gridView.GetColumnWidth(0) + gridView.GetColumnWidth(1) + 40));
	}

	~MainWindow()
	{

	}
};

class LogViewerApplication : public KApplication
{
public:

	int Main(KString **argv, int argc)
	{
		MainWindow wnd;
		wnd.SetVisible(true);

		::DoMessagePump(false);

		return 0;
	}
};

START_RFC_APPLICATION_NO_CMD_ARGS(LogViewerApplication);