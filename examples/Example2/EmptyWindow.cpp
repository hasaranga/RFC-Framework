
#include "../../amalgamated/rfc_amalgamated.h"


class MyApplication : public KApplication
{
public:

	int Main(KString **argv, int argc)
	{
		KFrame myWindow;

		myWindow.CreateComponent(); // this will create window

		myWindow.SetText(L"Empty Window"); // always use unicode strings. They are fast!
		myWindow.CenterScreen();
		myWindow.SetVisible(true);

		::DoMessagePump(); // message loop (this function will not return untill you call PostQuitMessage)

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication);