

// Creates a window with a button on it. Also shows how to use custom font for controls which automatically resizes when the dpi change.

#include "rfc/rfc.h"

class MyWindow : public KFrame
{
protected:
	KButton btn1;
	KFont fontSegoeUI;

public:
	MyWindow()
	{
		setText(L"My Window");

		// create the window before you add child controls to it!
		// after calling Create, you need to consider dpi scale when calling SetSize.
		create();

		// create font for controls after creating the window. So getDPI will return the current dpi of the screen and the font size will be resized.
		// can use the same font object for multiple controls if they use the same font size.
		// use precisely scaled fonts. for example, 20pt font is exactly twice the height and width of 10pt font. "Segoe UI" is a good choice.
		fontSegoeUI.load(L"Segoe UI", 14, false, false, false, true, this->GetDPI());

		// set size,font and position of the controls before adding them to window. 
		// so they will automatically rescale/reposition according to the current dpi.
		btn1.setPosition(10, 10);
		btn1.setText(L"My Button");

		btn1.onClick = [this](KButton* button) {
			::MessageBoxW(this->getHWND(), L"Hello World!", L"Welcome", MB_ICONINFORMATION);		
		};

		// we need to set custom font for standard controls to scale properly when dpi changes.
		// when the dpi change, font size will be automatically resized by the control if it has a custom font.
		// (control will not resize the default system font. remove the following line and see the effect when the dpi change!)
		btn1.setFont(fontSegoeUI); 

		addComponent(btn1);
	}
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