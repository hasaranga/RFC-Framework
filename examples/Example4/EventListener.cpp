
#include "../../amalgamated/rfc_amalgamated.h"


class MyWindow : public KFrame, public KButtonListener
{
protected:
	KButton btn1;

public:
	MyWindow()
	{
		this->SetText(L"My Window");
		this->Create();

		btn1.SetPosition(10, 10);
		btn1.SetText(L"My Button");
		btn1.SetListener(this); // set MyWindow class as button listener

		this->AddComponent(&btn1);
	}

	void OnButtonPress(KButton *button)
	{
		if (button == &btn1)
		{
			::MessageBoxW(this->GetHWND(), L"Button pressed!", L"Info", MB_ICONINFORMATION);
		}
	}
};

class MyApplication : public KApplication
{
public:
	int Main(KString **argv, int argc)
	{
		MyWindow window;

		window.CenterScreen();
		window.SetVisible(true);

		::DoMessagePump();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication);