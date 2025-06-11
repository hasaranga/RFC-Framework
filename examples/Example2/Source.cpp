

// Creates an empty window with high dpi support.

#include "rfc/rfc.h"

class MyApplication : public KApplication
{
public:

	int main(wchar_t** argv, int argc)
	{
		KFrame myWindow;

		myWindow.create(); // this will create window

		myWindow.setText(L"Empty Window"); // always use unicode strings. They are fast!
		myWindow.centerScreen();
		myWindow.setVisible(true);

		KApplication::messageLoop(); // this function will not return until you call PostQuitMessage

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);