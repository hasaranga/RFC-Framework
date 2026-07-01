

// Creates a window with a button on it.

#include "rfc/rfc.h"

class MyWindow : public KFrame
{
protected:
	KButton btn1;

public:
	MyWindow() noexcept
	{
		setText(L"My Window");
		addComponent(btn1);

		btn1.setPosition(10, 10);
		btn1.setText(L"My Button");

		btn1.onClick = [this](KButton* button) {
			::MessageBoxW(this->getHWND(), L"Hello World!", L"Welcome", MB_ICONINFORMATION);
		};

		create();
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