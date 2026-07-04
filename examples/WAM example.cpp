

#include "rfc.h"

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

class MyWindow : public KDrawable<KFrame>, public KAnimationEventListener
{
protected:
	ID2D1Factory* d2dFactory = nullptr;
	ID2D1HwndRenderTarget* renderTarget = nullptr;
	ID2D1SolidColorBrush* whiteBrush = nullptr;
	ID2D1SolidColorBrush* grayBrush = nullptr;

	KScopedClassPointer<KAnimationManager> animationManager;
	KComPtr<IUIAnimationVariable2> animVar1;
	KComPtr<IUIAnimationStoryboard2> storyboard;

	LRESULT onWMPaint(WPARAM wParam, LPARAM lParam) override
	{
		animationManager->update();

		PAINTSTRUCT ps;
		BeginPaint(compHWND, &ps);

		createDeviceResources();

		renderTarget->BeginDraw();

		// Clear background
		renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// Draw animated rectangle
		int value;
		animVar1->GetIntegerValue(&value);

		D2D1_RECT_F rectShape = D2D1::RectF(
			static_cast<FLOAT>(value),
			10.0f,
			static_cast<FLOAT>(value + 40),
			40.0f
		);

		renderTarget->FillRectangle(rectShape, grayBrush);

		HRESULT hr = renderTarget->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET)
		{
			discardDeviceResources();
		}

		EndPaint(compHWND, &ps);

		if (storyboard.get())
		{
			if (KAnimationManager::isStoryboardRunning(storyboard.get()))
				::InvalidateRect(compHWND, NULL, FALSE);
		}

		return 0;
	}

	LRESULT onLeftClick(WPARAM wParam, LPARAM lParam)
	{
		// stop animation
		if (storyboard.get())
			storyboard->Conclude();

		int currentValue;
		int nextValue;
		animVar1->GetIntegerValue(&currentValue);
		nextValue = (currentValue != 10) ? 10 : 200;

		KComPtr<IUIAnimationTransition2> transition = animationManager->createAccelerateDecelerateTransition(nextValue);
		storyboard = animationManager->createStoryboard();
		storyboard->AddTransition(animVar1.get(), transition.get());

		animationManager->scheduleStoryboard(storyboard.get());
		return 0;
	}

	void createDeviceResources()
	{
		if (renderTarget) return;

		RECT rc;
		::GetClientRect(compHWND, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// Create RenderTarget
		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRTProps =
			D2D1::HwndRenderTargetProperties(compHWND, size);

		HRESULT hr = d2dFactory->CreateHwndRenderTarget(
			rtProps,
			hwndRTProps,
			&renderTarget
		);

		if (SUCCEEDED(hr))
		{
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush);
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &grayBrush);
		}
	}

	void discardDeviceResources()
	{
		if (whiteBrush) { whiteBrush->Release(); whiteBrush = nullptr; }
		if (grayBrush) { grayBrush->Release(); grayBrush = nullptr; }
		if (renderTarget) { renderTarget->Release(); renderTarget = nullptr; }
	}

public:
	MyWindow()
	{
		animationManager = KAnimationManager::getInstance();
		animVar1 = animationManager->createVariable(10);
		animationManager->addEventListener(this);

		D2D1_FACTORY_OPTIONS options = {};
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &d2dFactory);

		setText(L"My Window");
		create();
	}

	void onClose() override
	{
		animationManager->shutdown();
		__super::onClose();
	}

	void onAnimationStart() override
	{
		if (storyboard.get())
		{
			//if (KAnimationManager::isStoryboardRunning(storyboard.get()))
				::InvalidateRect(compHWND, NULL, FALSE);
		}
	}

	~MyWindow()
	{
		discardDeviceResources();
		if (d2dFactory) d2dFactory->Release();
	}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_LBUTTONDOWN, onLeftClick)
	END_KMSG_HANDLER
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