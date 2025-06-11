
// Shows how to use menubar and popup menus.

#include "rfc/rfc.h"

class MyWindow : public KFrame
{
protected:
	KMenuBar menuBar;
	KMenu mFile, mEdit, mHelp;
	KMenuItem miOpen, miExit, miCut, miCopy, miPaste, miAbout;

public:
	MyWindow()
	{
		setText(L"My Window");
		create();

		miOpen.setText(L"Open...");
		miExit.setText(L"Exit");
		miCut.setText(L"Cut");
		miCopy.setText(L"Copy");
		miPaste.setText(L"Paste");
		miAbout.setText(L"About...");

		miAbout.onPress = [this](KMenuItem* item) {
			::MessageBoxW(this->GetHWND(), L"RFC Menu Example.", L"About", MB_ICONINFORMATION);
		};

		miExit.onPress = [this](KMenuItem* item) {
			this->onClose(); // destroy window and quit from message loop!
		};

		// add menu items into menu
		mFile.addMenuItem(&miOpen);
		mFile.addSeperator();
		mFile.addMenuItem(&miExit);
		mEdit.addMenuItem(&miCut);
		mEdit.addSeperator();
		mEdit.addMenuItem(&miCopy);
		mEdit.addMenuItem(&miPaste);
		mHelp.addMenuItem(&miAbout);

		// add menu into menubar
		menuBar.addMenu(L"File", &mFile);
		menuBar.addMenu(L"Edit", &mEdit);
		menuBar.addMenu(L"Help", &mHelp);

		menuBar.addToWindow(this); // add menubar into the window
	}

	LRESULT onRClickWindow(WPARAM wParam, LPARAM lParam)
	{
		mEdit.popUpMenu(this->getHWND()); // show mEdit menu as popup
		return 0;
	}

	// macro to handle window messages...
	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_RBUTTONUP, onRClickWindow) // calls onRClickWindow method when WM_RBUTTONUP msg received
	END_KMSG_HANDLER

};

class MyApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc)
	{
		MyWindow window;

		window.centerScreen();
		window.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);