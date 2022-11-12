

// Creates an empty window with high dpi support.

#include "rfc/rfc.h"

class MyApplication : public KApplication
{
public:

	int Main(KString** argv, int argc)
	{
		KFrame myWindow;

		myWindow.Create(); // this will create window

		myWindow.SetText(L"Empty Window"); // always use unicode strings. They are fast!
		myWindow.CenterScreen();
		myWindow.SetVisible(true);

		KApplication::MessageLoop(); // this function will not return until you call PostQuitMessage

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);