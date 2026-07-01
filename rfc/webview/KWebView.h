
/*
	Copyright (C) 2013-2026 CrownSoft

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
#include <wrl.h>
#include <WebView2.h>
#include <functional>

// Right click on solution then select "Manage Nuget packages". install Microsoft.Web.WebView2.
// All methods must be called from gui thread.
// To remove dll dependency, edit .vcxproj file as below.
/*
  <PropertyGroup>
	<WebView2LoaderPreference>Static</WebView2LoaderPreference>
  </PropertyGroup>
</Project>
*/
class KWebView : public KDrawable<KChildControl>
{
protected:
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> webviewController;
	Microsoft::WRL::ComPtr<ICoreWebView2_3> webview;
	Microsoft::WRL::ComPtr<ICoreWebView2Settings> webviewSettings;

	LRESULT onWMSizeMsg(WPARAM wParam, LPARAM lParam) noexcept
	{
		if (webviewController)
		{
			RECT bounds;
			GetClientRect(compHWND, &bounds);
			webviewController->put_Bounds(bounds);
		}
		return 0;
	}

	void onPaint(HDC hDCMem, RECT* rect, const int width, const int height) noexcept override
	{
		::FillRect(hDCMem, rect, (HBRUSH)::GetStockObject(DKGRAY_BRUSH));
	}

public:
	std::function<void(wchar_t*)> onPostMessage;
	std::function<void()> afterInit; // after webview2 initialized
	std::function<void()> onDOMLoaded;

	KWebView() noexcept {}

	// can call after adding to a window.
	bool initWebView() noexcept
	{
		if (compHWND == NULL)
			return false;

		CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
			Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
					env->CreateCoreWebView2Controller(compHWND, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {

							webviewController = controller;
							Microsoft::WRL::ComPtr<ICoreWebView2> webview2;
							webviewController->get_CoreWebView2(webview2.GetAddressOf());
							HRESULT hr = webview2->QueryInterface(_uuidof(ICoreWebView2_3), (void**)webview.GetAddressOf());
							webview2 = nullptr; // release

							if (FAILED(hr))
							{
								MessageBoxW(0, L"ICoreWebView2_3 interface not available!", L"Error", MB_ICONERROR);
								return S_FALSE;
							}

							webview->get_Settings(&webviewSettings);
							webviewSettings->put_IsScriptEnabled(TRUE);
							webviewSettings->put_AreDefaultScriptDialogsEnabled(TRUE);
							webviewSettings->put_IsWebMessageEnabled(TRUE);

							RECT bounds;
							GetClientRect(compHWND, &bounds);
							webviewController->put_Bounds(bounds);

							EventRegistrationToken token;
							webview->add_WebMessageReceived(Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
								[this](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
									LPWSTR message;
									HRESULT hr = args->TryGetWebMessageAsString(&message);
									if (SUCCEEDED(hr))
									{
										if (onPostMessage)
											onPostMessage(message);
									}
									::CoTaskMemFree(message);
									return S_OK;
								}).Get(), &token);

							webview->add_DOMContentLoaded(Microsoft::WRL::Callback<ICoreWebView2DOMContentLoadedEventHandler>(
								[this](ICoreWebView2* webview, ICoreWebView2DOMContentLoadedEventArgs* args) -> HRESULT {
									if (onDOMLoaded)
										onDOMLoaded();
									return S_OK;
								}).Get(), &token);

							if (afterInit)
								afterInit();

							return S_OK;
						}
					).Get());
					return S_OK;
				}
			).Get()
		);
		return true;
	}

	void postMessage(const wchar_t* message) noexcept
	{
		if (webview)
			webview->PostWebMessageAsString(message);
	}

	void postJSONMessage(const wchar_t* message) noexcept
	{
		if (webview)
			webview->PostWebMessageAsJson(message);
	}

	Microsoft::WRL::ComPtr<ICoreWebView2Controller> getController() noexcept
	{
		return webviewController;
	}

	Microsoft::WRL::ComPtr<ICoreWebView2_3> getWebView() noexcept
	{
		return webview;
	}

	Microsoft::WRL::ComPtr<ICoreWebView2Settings> getSettings() noexcept
	{
		return webviewSettings;
	}

	// folderPath must be full path
	void setVirtualHostDir(const wchar_t* hostName, const wchar_t* folderPath) noexcept
	{
		if (webview)
			webview->SetVirtualHostNameToFolderMapping(hostName, folderPath,
				COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY_CORS);
	}

	void navigate(const wchar_t* url) noexcept
	{
		if (webview)
			HRESULT hr = webview->Navigate(url);
	}

	virtual ~KWebView() noexcept {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_SIZE, onWMSizeMsg)
	END_KMSG_HANDLER
};