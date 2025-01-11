#pragma once

#include "../core/CoreModule.h"
#include <windows.h>

// RTL v1.0
// Instead of handling all control messages(WM_COMMAND) within TWindow, we handle them within subclasses.
// Therefore, only required messages are handled, making it more lightweight.
// This framework also serves as a clear illustration of Win32 GUI concepts and HWND usage.
// It doesn't try to hide the underlying Win32 API, but rather provides a simple and easy-to-use wrapper around it.

// base class for anything which has HWND.
// can associate with HWND and call HWND related apis.
// does not destroy HWND on destruction.
class THwnd
{
protected:
    HWND hwnd;
    KString className;
    KString caption;
    HWND parentHWND;
    DWORD dwStyle;
    DWORD dwExStyle;
    int xPos;
    int yPos;
    int width;
    int height;
    bool visible;
    bool enabled;

public:
    THwnd()
    {
        xPos = 0;
        yPos = 0;
        width = 0;
        height = 0;
        dwStyle = 0;
        dwExStyle = 0;
        hwnd = 0;
        parentHWND = 0;
        visible = true;
        enabled = true;
    }

    virtual void SetHWND(HWND hwnd)
    {
        this->hwnd = hwnd;
    }

    operator HWND()const {
        return hwnd;
    }

    virtual KString GetClassName()
    {
        return className;
    }

    virtual KString GetText()
    {
        return caption;
    }

    virtual void SetText(const KString& text)
    {
        this->caption = text;
        if (hwnd)
            ::SetWindowTextW(hwnd, caption);
    }

    virtual void SetParentHWND(HWND parentHWND)
    {
        this->parentHWND = parentHWND;
        if (hwnd)
            ::SetParent(hwnd, parentHWND);
    }

    virtual HWND GetParentHWND()
    {
        return parentHWND;
    }

    virtual DWORD GetStyle()
    {
        return dwStyle;
    }

    virtual void SetStyle(DWORD style)
    {
        dwStyle = style;
        if (hwnd)
            ::SetWindowLongPtrW(hwnd, GWL_STYLE, dwStyle);
    }

    virtual DWORD GetExStyle()
    {
        return dwExStyle;
    }

    virtual void SetExStyle(DWORD exStyle)
    {
        dwExStyle = exStyle;
        if (hwnd)
            ::SetWindowLongPtrW(hwnd, GWL_EXSTYLE, dwExStyle);
    }

    virtual int GetX()
    {
        return xPos;
    }

    virtual int GetY()
    {
        return yPos;
    }

    virtual int GetWidth()
    {
        return width;
    }

    virtual int GetHeight()
    {
        return height;
    }

    virtual void SetSize(int width, int height)
    {
        this->width = width;
        this->height = height;

        if (hwnd)
        {
            ::SetWindowPos(hwnd, 0, 0, 0, width, height,
                SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
        }
    }

    virtual void SetPosition(int x, int y)
    {
        xPos = x;
        yPos = y;

        if (hwnd)
        {
            ::SetWindowPos(hwnd, 0, xPos, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
        }
    }

    virtual void SetVisible(bool visible)
    {
        this->visible = visible;
        if (hwnd)
            ::ShowWindow(hwnd, visible ? SW_SHOW : SW_HIDE);
    }

    virtual bool IsVisible()
    {
        return visible;
    }

    virtual bool IsEnabled()
    {
        return enabled;
    }

    virtual void SetEnabled(bool enable)
    {
        enabled = enable;

        if (hwnd)
            ::EnableWindow(hwnd, enabled ? TRUE : FALSE);
    }

    virtual void BringToFront()
    {
        if (hwnd)
            ::BringWindowToTop(hwnd);
    }

    virtual void GrabKeyboardFocus()
    {
        if (hwnd)
            ::SetFocus(hwnd);
    }

    virtual void Repaint()
    {
        if (hwnd)
        {
            ::InvalidateRect(hwnd, NULL, TRUE);
            ::UpdateWindow(hwnd); // instant update
        }
    }

    virtual void Destroy()
    {
        if (hwnd)
            ::DestroyWindow(hwnd);
    }

    virtual ~THwnd()
    {

    }
};

class TIDGenerator
{
private:
	static TIDGenerator* _instance;
	TIDGenerator();

protected:
	volatile int classCount;
	volatile int controlCount;

public:

	static TIDGenerator* GetInstance();

	UINT GenerateControlID();
	KString GenerateClassName();

	~TIDGenerator();
};

// T must be derived from THwnd
template <class T>
class TControl : public T
{
protected:
    UINT controlID;
public:
    TControl()
    {
        TIDGenerator* idGenerator = TIDGenerator::GetInstance();
        controlID = idGenerator->GenerateControlID();

        this->dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    }

    UINT GetControlID()
    {
        return controlID;
    }

    virtual void Create()
    {
        this->hwnd = ::CreateWindowExW(this->dwExStyle, this->className, this->caption,
            this->dwStyle, this->xPos, this->yPos, this->width, this->height,
            this->parentHWND, (HMENU)(UINT_PTR)this->controlID, KApplication::hInstance,
            (LPVOID)(TControl<T>*)this);

        if (!this->enabled)
            ::EnableWindow(this->hwnd, FALSE);

        if (!this->visible)
            ::ShowWindow(this->hwnd, SW_HIDE);
    }

    virtual ~TControl() {}
};

// creates an HWND and associates it with a window procedure.
// does not destroy HWND on destruction.
// T must be derived from THwnd
template <class T>
class TWindowProcImpl : public T
{
protected:
    WNDCLASSEXW wcEx;

	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        TWindowProcImpl<T>* pThis = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (TWindowProcImpl<T>*)pCreate->lpCreateParams;
            ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->SetHWND(hwnd);
        }
        else
        {
            pThis = (TWindowProcImpl<T>*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        }

        if (pThis)
            return pThis->WindowProc(hwnd, uMsg, wParam, lParam);
        else
            return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

    virtual LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

public:
    TWindowProcImpl()
	{
        TIDGenerator* idGenerator = TIDGenerator::GetInstance();
        this->className = idGenerator->GenerateClassName();

        wcEx.cbSize = sizeof(WNDCLASSEXW);
        wcEx.hCursor = ::LoadCursorW(NULL, IDC_ARROW);
        wcEx.hIcon = 0;
        wcEx.lpszMenuName = 0;
        wcEx.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
        wcEx.cbClsExtra = 0;
        wcEx.cbWndExtra = 0;
        wcEx.hIconSm = 0;
        wcEx.style = 0;
        wcEx.hInstance = KApplication::hInstance;
        wcEx.lpszClassName = this->className;

        wcEx.lpfnWndProc = StaticWindowProc;
	}

	virtual bool Create()
	{
        if (!::RegisterClassExW(&wcEx))
            return false;

        ::CreateWindowExW(this->dwExStyle, this->className, this->caption,
            this->dwStyle, this->xPos, this->yPos, this->width, this->height,
            this->parentHWND, 0, KApplication::hInstance, (LPVOID)(TWindowProcImpl<T>*)this);

        if(!this->enabled)
            ::EnableWindow(this->hwnd, FALSE);

        if(this->visible)
            ::ShowWindow(this->hwnd, SW_SHOW);

        return true;
	}

	virtual ~TWindowProcImpl() {}
};

// creates a control and subclass it with a window procedure.
// does not destroy HWND on destruction.
// T must be derived from TControl.
// ex: class CustomButton : public TSubclassImpl<CButton>{};
template <class T>
class TSubclassImpl : public T
{
protected:
    WNDPROC defaultProc;

	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        TSubclassImpl<T>* pThis = (TSubclassImpl<T>*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

        if (pThis)
            return pThis->WindowProc(hwnd, uMsg, wParam, lParam);
        else
            return 0; // never happen.
	}

    virtual LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if(uMsg == NC_DESTROY)
            ::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)defaultProc);

        return ::CallWindowProc(defaultProc, hwnd, uMsg, wParam, lParam);
    }

public:
    TSubclassImpl(){}

	virtual void Create() override
	{
        T::Create();

        defaultProc = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);

        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)(TSubclassImpl<T>*)this);
        ::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)StaticWindowProc);

        return true;
	}

	virtual ~TSubclassImpl() {}
};


// creates a basic top level window and implements few window functions.
// does not destroy HWND on destruction.
// T must be derived from TWindowProcImpl
template <class T>
class TWindow : public T
{
protected:
    virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
    {
        switch (msg)
        {
            case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window...
                {
                    RECT rect;
                    ::GetWindowRect(hwnd, &rect);

                    this->width = rect.right - rect.left;
                    this->height = rect.bottom - rect.top;
                }
                return T::WindowProc(hwnd, msg, wParam, lParam);

            case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
                {
                    RECT rect;
                    ::GetWindowRect(hwnd, &rect);

                    this->xPos = rect.left;
                    this->yPos = rect.top;
                }
                return T::WindowProc(hwnd, msg, wParam, lParam);

            default:
                return T::WindowProc(hwnd, msg, wParam, lParam);
        }

        return 0;
    }

public:
    TWindow()
    {
        this->width = 400;
        this->height = 200;
        this->visible = false;

        this->dwStyle = WS_POPUP;
        this->dwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
        this->wcEx.style = CS_HREDRAW | CS_VREDRAW;
    }

    virtual void Flash()
    {
        if(this->hwnd)
        ::FlashWindow(this->hwnd, TRUE);
    }

    virtual void CenterScreen()
    {
        this->SetPosition((::GetSystemMetrics(SM_CXSCREEN) - this->width) / 2,
            (::GetSystemMetrics(SM_CYSCREEN) - this->height) / 2);
    }

    virtual void CenterOnSameMonitor(HWND window)
    {
        if (window)
        {
            HMONITOR hmon = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

            if (hmon != NULL)
            {
                MONITORINFO monitorInfo;
                ::ZeroMemory(&monitorInfo, sizeof(MONITORINFO));
                monitorInfo.cbSize = sizeof(MONITORINFO);

                if (::GetMonitorInfoW(hmon, &monitorInfo))
                {
                    const int posX = monitorInfo.rcMonitor.left + 
                        (((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) - this->width) / 2);

                    const int posY = monitorInfo.rcMonitor.top + 
                        (((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) - this->height) / 2);

                    this->SetPosition(posX, posY);

                    return;
                }
            }
        }

        this->CenterScreen();
    }

    virtual void SetClientAreaSize(int width, int height)
    {
        if (this->hwnd)
        {
            RECT wndRect;
            ::GetClientRect(this->hwnd, &wndRect);

            wndRect.right = wndRect.left + width;
            wndRect.bottom = wndRect.top + height;

            ::AdjustWindowRect(&wndRect, this->dwStyle, ::GetMenu(this->hwnd) == NULL ? FALSE : TRUE);
            this->SetSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);
        }
    }

    virtual bool IsOffScreen(int posX, int posY)
    {
        POINT point;
        point.x = posX;
        point.y = posY;
        return (::MonitorFromPoint(point, MONITOR_DEFAULTTONULL) == NULL);
    }
};

// handles common window events.
// T must be derived from TWindow
template <class T>
class TWindowEventHandler : public T
{
protected:
    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnSize(int width, int height) {}
    virtual void OnMove(int x, int y) {}
    virtual void OnActivate() {}
    virtual void OnDeactivate() {}
    virtual void OnClose() {}
    virtual void OnShow() {}
    virtual void OnHide() {}

    virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
    {
        switch (msg)
        {
        case WM_CREATE:
            this->OnCreate();
            break;
        case WM_DESTROY:
            this->OnDestroy();
            break;
        case WM_SIZE:
            this->OnSize(LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_MOVE:
            this->OnMove(LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_ACTIVATE:
            if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
                this->OnActivate();
            else
                this->OnDeactivate();
            break;
        case WM_CLOSE:
            this->OnClose();
            break;
        case WM_SHOWWINDOW:
            if (wParam == TRUE)
                this->OnShow();
            else
                this->OnHide();
            break;
        }
        return T::WindowProc(hwnd, msg, wParam, lParam);
    }

public:
    TWindowEventHandler() {}
    virtual ~TWindowEventHandler() {}
};

// T must be derived from TWindowEventHandler
template <class T>
class TDestroyAndQuitOnClose : public T
{
protected:
    virtual void OnClose() override
    {
        this->SetVisible(false);
        this->Destroy();
    }

    virtual void OnDestroy() override
    {
        ::PostQuitMessage(0);
    }

public:
    TDestroyAndQuitOnClose(){}
    virtual ~TDestroyAndQuitOnClose() {}
};

// T must be derived from TWindowEventHandler
template <class T>
class THideOnClose : public T
{
protected:
    virtual void OnClose() override
    {
        this->SetVisible(false);
    }

public:
    THideOnClose() {}
    virtual ~THideOnClose() {}
};

// T must be derived from TWindow
template <class T>
class TResizableWindow : public T
{
public:
    TResizableWindow()
    {
        this->caption.AssignStaticText(TXT_WITH_LEN("TResizableWindow"));
        this->dwStyle = WS_OVERLAPPEDWINDOW;
    }

    virtual ~TResizableWindow(){}
};

class CResizableWindow : public TDestroyAndQuitOnClose<TWindowEventHandler<TResizableWindow<TWindow<TWindowProcImpl<THwnd>>>>>
{
public:
    CResizableWindow() {}
    virtual ~CResizableWindow() {}
};

// Catches a button press event by processing the WM_COMMAND message of TWindow.
// T must be derived from TWindow
template <class T>
class TButtonHandler : public T
{
protected:
    virtual void OnButtonPress(UINT buttonId){ }

    virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
    {
        if ((msg == WM_COMMAND) && (HIWORD(wParam) == BN_CLICKED))
        {
            this->OnButtonPress(LOWORD(wParam));
            return 0;
        }
        return T::WindowProc(hwnd, msg, wParam, lParam);
    }

public:
    TButtonHandler() {}
    virtual ~TButtonHandler() {}
};

// creates button.
// T must be derived from THwnd
template <class T>
class TButton : public TControl<T>
{
public:
    TButton()
    {
        this->className.AssignStaticText(TXT_WITH_LEN("BUTTON"));
        this->caption.AssignStaticText(TXT_WITH_LEN("Button"));

        this->width = 100;
        this->height = 30;

        this->dwStyle |= BS_NOTIFY | WS_TABSTOP;
        this->dwExStyle = WS_EX_WINDOWEDGE;
    }

    virtual ~TButton(){}
};

class CButton : public TButton<THwnd>
{
public:
    CButton() {}
    virtual ~CButton() {}
};

// creates single line edit control.
// T must be derived from THwnd
template <class T>
class TTextBox : public TControl<T>
{
public:
    TTextBox()
    {
        this->className.AssignStaticText(TXT_WITH_LEN("EDIT"));
        this->caption.AssignStaticText(TXT_WITH_LEN(""));

        this->width = 200;
        this->height = 20;

        this->dwStyle |= WS_TABSTOP | ES_AUTOHSCROLL;
        this->dwExStyle = WS_EX_CLIENTEDGE;
    }

    virtual ~TTextBox(){}

    virtual KString GetText() override
    {
        if (this->hwnd)
        {
            const int length = ::GetWindowTextLengthW(this->hwnd);
            if (length > 0)
            {
                wchar_t* buffer = new wchar_t[length + 1];
                ::GetWindowTextW(this->hwnd, buffer, length + 1);
                this->caption = KString(buffer);
                delete[] buffer;
            }
            else
            {
                this->caption = KString();
            }           
        }
        return this->caption;
    }
};

class CTextBox : public TTextBox<THwnd>
{
public:
    CTextBox() {}
    virtual ~CTextBox() {}
};

