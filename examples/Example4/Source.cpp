
// Shows how to use menubar and popup menus.

#include "rfc/rfc.h"

class MyWindow : public KFrame , public KMenuItemListener
{
protected:
	KMenuBar menuBar;
	KMenu mFile, mEdit, mHelp;
	KMenuItem miOpen, miExit, miCut, miCopy, miPaste, miAbout;

public:
	MyWindow()
	{
		this->SetText(L"My Window");
		this->Create();

		miOpen.SetText(L"Open...");
		miExit.SetText(L"Exit");
		miCut.SetText(L"Cut");
		miCopy.SetText(L"Copy");
		miPaste.SetText(L"Paste");
		miAbout.SetText(L"About...");

		miOpen.SetListener(this);
		miExit.SetListener(this);
		miCut.SetListener(this);
		miCopy.SetListener(this);
		miPaste.SetListener(this);
		miAbout.SetListener(this);

		// add menu items into menu
		mFile.AddMenuItem(&miOpen);
		mFile.AddSeperator();
		mFile.AddMenuItem(&miExit);
		mEdit.AddMenuItem(&miCut);
		mEdit.AddSeperator();
		mEdit.AddMenuItem(&miCopy);
		mEdit.AddMenuItem(&miPaste);
		mHelp.AddMenuItem(&miAbout);

		// add menu into menubar
		menuBar.AddMenu(L"File", &mFile);
		menuBar.AddMenu(L"Edit", &mEdit);
		menuBar.AddMenu(L"Help", &mHelp);

		menuBar.AddToWindow(this); // add menubar into the window
	}

	void OnMenuItemPress(KMenuItem* menuItem) override
	{
		if (menuItem == &miAbout)
		{
			::MessageBoxW(this->GetHWND(), L"RFC Menu Example.", L"About", MB_ICONINFORMATION);
		}
		else if (menuItem == &miExit)
		{
			this->OnClose(); // destroy window and quit from message loop!
		}
	}

	LRESULT OnRClickWindow(WPARAM wParam, LPARAM lParam)
	{
		mEdit.PopUpMenu(this); // show mEdit menu as popup
		return 0;
	}

	// macro to handle window messages...
	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_RBUTTONUP, OnRClickWindow) // calls OnRClickWindow method when WM_RBUTTONUP msg received
	END_KMSG_HANDLER

	~MyWindow()
	{

	}
};

class MyApplication : public KApplication
{
public:
	int Main(KString** argv, int argc)
	{
		MyWindow window;

		window.CenterScreen();
		window.SetVisible(true);

		KApplication::MessageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);