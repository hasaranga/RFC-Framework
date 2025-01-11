
// Shows how to create and draw a custom component.

#include "rfc/rfc.h"

class CustomComponent : public KComponent
{
protected:
	COLORREF colBTNFACE, colBTNFRAME;
	KScopedGdiObject<HBRUSH> hFaceBrush, hFrameBrush;
	KScopedClassPointer<KFont> font1;

public:
	CustomComponent() : KComponent(true) // we are generating a new class name.
	{
		this->SetStyle(WS_CHILD); // don't forget this!
		this->SetSize(100, 25);
		this->SetText(L"Hello World");

		colBTNFACE = RGB(147, 196, 255);
		colBTNFRAME = RGB(89, 164, 255);

		hFaceBrush = ::CreateSolidBrush(colBTNFACE);
		hFrameBrush = ::CreateSolidBrush(colBTNFRAME);

		font1 = new KFont(L"Courier New", 14, false, false, false, true, USER_DEFAULT_SCREEN_DPI);
		this->SetFont(font1);
	}

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam)
	{
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rect;

		::GetClientRect(compHWND, &rect);
		const int width = rect.right - rect.left;
		const int height = rect.bottom - rect.top;

		hdc = ::BeginPaint(compHWND, &ps);

		// double buffering
		HDC memHDC = ::CreateCompatibleDC(hdc);
		HBITMAP memBMP = ::CreateCompatibleBitmap(hdc, width, height);;
		::SelectObject(memHDC, memBMP);
		
		::FillRect(memHDC, &rect, hFaceBrush);
		::FrameRect(memHDC, &rect, hFrameBrush);

		HGDIOBJ oldFont = ::SelectObject(memHDC, font1->GetFontHandle());
		COLORREF oldTextColor = ::SetTextColor(memHDC, RGB(255, 255, 255));
		int oldBkMode = ::SetBkMode(memHDC, TRANSPARENT);

		::DrawTextW(memHDC, compText, compText.GetLength(), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		::BitBlt(hdc, 0, 0, width, height, memHDC, 0, 0, SRCCOPY);

		::SetBkMode(memHDC, oldBkMode);
		::SetTextColor(memHDC, oldTextColor);
		::SelectObject(memHDC, oldFont);

		::DeleteDC(memHDC);
		::DeleteObject(memBMP);

		::EndPaint(compHWND, &ps);

		return 0;
	}

	LRESULT OnEraseBackground(WPARAM wParam, LPARAM lParam)
	{
		return 1; // avoids flickering
	}

	virtual ~CustomComponent() {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_PAINT, OnPaint)
		ON_KMSG(WM_ERASEBKGND, OnEraseBackground)
	END_KMSG_HANDLER

};

class ShinyGUI : public KFrame
{
protected:
	CustomComponent customComp;

public:
	ShinyGUI()
	{
		this->Create();
		this->SetText(L"ShinyGUI");

		customComp.SetPosition(50, 50);

		this->AddComponent(&customComp);
	}

};

class MyGreatApp : public KApplication
{

public:
	int Main(KString** argv, int argc)
	{
		ShinyGUI mainWnd;

		mainWnd.CenterScreen();
		mainWnd.SetVisible(true);

		KApplication::MessageLoop();

		return 0;
	}
};

START_RFC_APPLICATION_NO_CMD_ARGS(MyGreatApp, KDPIAwareness::STANDARD_MODE)