

// Creates a window with a button on it. Also shows how to use custom font for controls which automatically resizes when the dpi change.

#include "rfc/rfc.h"

class MyWindow : public KFrame , public KButtonListener
{
protected:
	KButton btn1;
	KFont* fontSegoeUI;

public:
	MyWindow()
	{
		this->SetText(L"My Window");
		this->Create(); // create the window before you add child controls to it!

		// create font for controls after creating the window. So GetDPI will return the current dpi of the screen and the font size will be resized.
		// can use the same font object for multiple controls if they use the same font size.
		// use precisely scaled fonts. for example, 20pt font is exactly twice the height and width of 10pt font. "Segoe UI" is a good choice.
		fontSegoeUI = new KFont(L"Segoe UI", 14, false, false, false, true, this->GetDPI());

		// set size,font and position of the controls before adding them to window. 
		// so they will automatically rescale/reposition according to the current dpi.
		btn1.SetPosition(10, 10);
		btn1.SetText(L"My Button");
		btn1.SetListener(this); // set MyWindow class as the button listener

		// we need to set custom font for standard controls to scale properly when dpi changes.
		// when the dpi change, font size will be automatically resized by the control if it has a custom font.
		// (control will not resize the default system font. remove the following line and see the effect when the dpi change!)
		btn1.SetFont(fontSegoeUI); 

		this->AddComponent(&btn1);
	}

	void OnButtonPress(KButton* button) override
	{
		if (button == &btn1)
		{
			MessageBoxW(this->GetHWND(), L"Hello World!", L"Welcome", MB_ICONINFORMATION);
		}
	}

	~MyWindow()
	{
		delete fontSegoeUI;
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