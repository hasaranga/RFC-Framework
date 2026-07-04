
// Shows how to create and draw a custom component.

#include "rfc.h"

class CustomComponent : public KDrawable<KChildControl>
{
protected:
	COLORREF colBTNFACE, colBTNFRAME;
	KScopedGdiObject<HBRUSH> hFaceBrush, hFrameBrush;

	virtual void onPaint(HDC hDCMem, const RECT& clientRect, 
		const Physical width, const Physical height) noexcept override
	{
		// let's use gdi apis to draw on hDCMem(double buffered)

		::FillRect(hDCMem, &clientRect, hFaceBrush);
		::FrameRect(hDCMem, &clientRect, hFrameBrush);

		HGDIOBJ oldFont = ::SelectObject(hDCMem, compFontRef.getFontHandle());
		COLORREF oldTextColor = ::SetTextColor(hDCMem, RGB(255, 255, 255));
		int oldBkMode = ::SetBkMode(hDCMem, TRANSPARENT);

		RECT rect = clientRect;
		::DrawTextW(hDCMem, compText, compText.length(), &rect, 
			DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		// restore
		::SetBkMode(hDCMem, oldBkMode);
		::SetTextColor(hDCMem, oldTextColor);
		::SelectObject(hDCMem, oldFont);
	}

public:
	CustomComponent() noexcept
	{
		setSize(100, 25);
		setText(L"Hello World");

		colBTNFACE = RGB(147, 196, 255);
		colBTNFRAME = RGB(89, 164, 255);

		hFaceBrush = ::CreateSolidBrush(colBTNFACE);
		hFrameBrush = ::CreateSolidBrush(colBTNFRAME);

		setFontType(KFontType(L"Courier New", 14));
	}
};

class ShinyGUI : public KFrame
{
protected:
	CustomComponent customComp;

public:
	ShinyGUI() noexcept
	{
		setText(L"ShinyGUI");
		create();	

		customComp.setPosition(50, 50);
		addComponent(customComp);
	}

};

class MyGreatApp : public KApplication
{
public:
	int main(wchar_t** argv, int argc) noexcept
	{
		ShinyGUI mainWnd;

		mainWnd.centerScreen();
		mainWnd.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION_NO_CMD_ARGS(MyGreatApp, KDPIAwareness::STANDARD_MODE)