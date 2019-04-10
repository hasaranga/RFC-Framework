
#include "../../amalgamated/rfc_amalgamated.h"


class MyWindow : public KFrame
{
protected:
	KButton btn1;

public:
	MyWindow()
	{
		this->SetText(L"My Window");
		this->Create(); // create window before you add child components to it!
		
		btn1.SetPosition(10, 10);
		btn1.SetText(L"My Button");

		this->AddComponent(&btn1);
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