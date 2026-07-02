

// Creates a window with a button on it. pressing button will show the second window.
// demonstrate the use of close operation.

#include "rfc/rfc.h"

class MyWindow : public KFrame
{
protected:
	KButton btn1;
	KFrame aboutWindow;

public:
	MyWindow() noexcept
	{
		setText(L"My Window");
		create();

		aboutWindow.setParentHWND(getHWND()); // parent must created first to get HWND.
		aboutWindow.setSize(100, 50);

		// using KCloseOperation::DestroyAndExit will destroy the window and exit from message loop.
		// the default close operation of a window is KCloseOperation::DestroyAndExit.
		// KCloseOperation::Hide option just hide the window. it does not destroy or quit the message loop.
		// you can also override KWindow::onClose, KWindow::onDestroy methods to change closing behaviour.
		aboutWindow.setCloseOperation(KCloseOperation::Hide);

		aboutWindow.create();

		btn1.setPosition(10, 10);
		btn1.setText(L"About");

		btn1.onClick = [this](KButton* button) {
			aboutWindow.setVisible(true);
			aboutWindow.bringToFront();
		};
	
		addComponent(btn1);
	}
};

class MyApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc) noexcept
	{
		MyWindow window;

		window.centerScreen();
		window.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);