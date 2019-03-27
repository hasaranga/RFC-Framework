

#include "../../amalgamated/rfc_amalgamated.h"

class CustomComponent : public KComponent
{
protected:
	COLORREF colBTNFACE, colBTNFRAME;
	HBRUSH  hFaceBrush, hFrameBrush;
	KFont* font1;

public:
	CustomComponent() : KComponent(true)
	{
		this->SetStyle(WS_CHILD); // don't forget this!
		this->SetSize(100, 25);
		this->SetText(L"Hello World");

		colBTNFACE = RGB(147, 196, 255);
		colBTNFRAME = RGB(89, 164, 255);

		hFaceBrush = ::CreateSolidBrush(colBTNFACE);
		hFrameBrush = ::CreateSolidBrush(colBTNFRAME);

		font1 = new KFont(L"Courier New", 14);
	}

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam) // draw without double buffering! coz i'm lazy ;-)
	{
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rect;

		hdc = ::BeginPaint(compHWND, &ps);
		::GetClientRect(compHWND, &rect);

		::FillRect(hdc, &rect, hFaceBrush);
		::FrameRect(hdc, &rect, hFrameBrush);

		HGDIOBJ oldFont = ::SelectObject(hdc, font1->GetFontHandle());
		COLORREF oldTextColor = ::SetTextColor(hdc, RGB(255, 255, 255));
		int oldBkMode = ::SetBkMode(hdc, TRANSPARENT);

		::DrawTextW(hdc, compText, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		::SetBkMode(hdc, oldBkMode);
		::SetTextColor(hdc, oldTextColor);
		::SelectObject(hdc, oldFont);

		::EndPaint(compHWND, &ps);

		return 0;
	}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_PAINT, OnPaint)
	END_KMSG_HANDLER(KComponent)

	~CustomComponent()
	{
		::DeleteObject(hFaceBrush);
		::DeleteObject(hFrameBrush);

		delete font1;
	}
};

class ShinyGUI : public KFrame
{
protected:
	CustomComponent customComp;

public:
	ShinyGUI()
	{
		this->CreateComponent();
		this->SetText(L"ShinyGUI");

		customComp.SetPosition(50, 50);

		this->AddComponent(&customComp);
	}

};

class MyGreatApp : public KApplication
{

public:
	int Main(KString **argv, int argc)
	{
		ShinyGUI mainWnd;

		mainWnd.CenterScreen();
		mainWnd.SetVisible(true);

		::DoMessagePump();

		return 0;
	}
};

START_RFC_APPLICATION_NO_CMD_ARGS(MyGreatApp)