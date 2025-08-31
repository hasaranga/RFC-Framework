
/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include "../gui/GUIModule.h"
#include "qrcodegen/qrcodegen.h"

// automatically scales properly on dpi change.
// width and height must be equal.
class KQRCodeBox : public KComponent
{
protected:
	uint8_t qr0[qrcodegen_BUFFER_LEN_MAX];
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	COLORREF qrColor = RGB(0, 0, 0);
	bool generated = false;

	virtual LRESULT onPaint(WPARAM wParam, LPARAM lParam)
	{
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rect;

		::GetClientRect(compHWND, &rect);
		const int width = rect.right - rect.left;
		const int height = rect.bottom - rect.top;

		hdc = ::BeginPaint(compHWND, &ps);

		if (!generated)
		{
			::FillRect(hdc, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
			::EndPaint(compHWND, &ps);
			return 0;
		}

		HDC memHDC = ::CreateCompatibleDC(hdc);
		int qrSize = qrcodegen::qrcodegen_getSize(qr0);

		// now we need to access raw pixel data.
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = qrSize;
		bmi.bmiHeader.biHeight = -qrSize; // top-down
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		void* pvBits; // the color format for each pixel is 0xaarrggbb  
		HBITMAP memBMP = ::CreateDIBSection(memHDC, &bmi,
			DIB_RGB_COLORS, &pvBits,
			NULL, 0);

		for (int y = 0; y < qrSize; y++) {
			for (int x = 0; x < qrSize; x++) {
				bool isBlack = qrcodegen::qrcodegen_getModule(qr0, x, y);
				((unsigned int*)pvBits)[x + y * qrSize] = isBlack ? (qrColor | 0xFF000000) : 0xFFFFFFFF;
			}
		}

		HBITMAP hbmPrev = (HBITMAP)::SelectObject(memHDC, memBMP);
		::StretchBlt(hdc, 0, 0, width, height, memHDC, 0, 0, qrSize, qrSize, SRCCOPY);

		::SelectObject(memHDC, hbmPrev);
		::DeleteObject(memBMP);
		::DeleteDC(memHDC);

		::EndPaint(compHWND, &ps);
		return 0;
	}

	LRESULT onEraseBackground(WPARAM wParam, LPARAM lParam)
	{
		return 1; // avoids flickering
	}

public:
	KQRCodeBox() : KComponent(true)
	{
		setStyle(WS_CHILD);
		setSize(100, 100);
		compText = L"QRCode";
	}

	void setColor(COLORREF qrColor)
	{
		this->qrColor = qrColor;
		repaint();
	}

	virtual void setText(const KString& compText) override
	{
		char* utf8Str = KString::toAnsiString(compText);

		qrcodegen::qrcodegen_encodeText(utf8Str,
			tempBuffer, qr0, qrcodegen::qrcodegen_Ecc_MEDIUM,
			qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
			qrcodegen::qrcodegen_Mask_AUTO, true);

		::free(utf8Str);
		generated = true;

		this->compText = compText; // no need to update window text!
		repaint();
	}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_PAINT, onPaint)
		ON_KMSG(WM_ERASEBKGND, onEraseBackground)
	END_KMSG_HANDLER
};
