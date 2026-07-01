
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

// compatibility : win8.1 or higher
// gpu based fluid animations.
// no mechanism to inform the end of animation. (dcomposition limitation)
// you have to poll the animation variable.
// use these classes only if you want to do animations with panels.

#pragma once

#include "../core/CoreModule.h"
#include "../gui/GUIModule.h"

#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dcomp.h>
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <uianimation.h>

namespace composition {

    // can be use to represent a group of controls.
    // holds a reference to visual,surface and their properties.
    // value of properties will remain between resource recreation and they will reassigned.
    class Layer
    {
    protected:
        Physical x = 0;
        Physical y = 0;
        Physical width = 100;
        Physical height = 100;
        bool visible = true;
        IDCompositionVisual2* visual = nullptr;
        IDCompositionSurface* surface = nullptr;
        IDCompositionDesktopDevice* compositionDevice = nullptr;
        ID2D1DeviceContext* d2dDeviceContext = nullptr;
        Layer* parentLayer = nullptr;
        IDCompositionVisual2* rootVisual = nullptr;
        D2D1::ColorF backgroundColor = D2D1::ColorF(0,0.0f); // transparent

        virtual void recreateSurface() noexcept
        {
            if (surface)
            {
                surface->Release();
                surface = nullptr;
            }

            compositionDevice->CreateSurface(width,
                height,
                DXGI_FORMAT_B8G8R8A8_UNORM,
                DXGI_ALPHA_MODE_PREMULTIPLIED,
                &surface);
        }

    public:

        Layer() noexcept {}
        virtual ~Layer() noexcept {}

        IDCompositionVisual2* getVisual() noexcept { return visual; }
        IDCompositionSurface* getSurface() noexcept { return surface; }

        void removeFromParent() noexcept
        {
            IDCompositionVisual2* parentVisual = nullptr;
            if (parentLayer)
            {
                parentVisual = parentLayer->getVisual();
            }
            else // no parent layer. so, rootVisual is the parent.
            {
                parentVisual = rootVisual;
            }

            if ((parentVisual != nullptr) && (visual != nullptr))
            {
               parentVisual->RemoveVisual(visual);
            }

            parentLayer = nullptr;
        }

        // MSDN: Do not hide visuals by setting opacity to 0% or clipping; instead, remove visuals from the visual tree.
        void setVisible(bool visible) noexcept
        {
            if (this->visible == visible)
                return;

            this->visible = visible;

            IDCompositionVisual2* parentVisual = nullptr;
            if (parentLayer)
            {
                parentVisual = parentLayer->getVisual();
            }
            else // no parent layer. so, rootVisual is the parent.
            {
                parentVisual = rootVisual;
            }

            if ((parentVisual != nullptr) && (visual != nullptr))
            {
                if (visible)
                    parentVisual->AddVisual(visual, FALSE, nullptr);
                else
                    parentVisual->RemoveVisual(visual);
            }
        }

        bool isVisible() noexcept { return visible; }

        void setBackgroundColor(const D2D1::ColorF& color) noexcept
        {
            backgroundColor = color;

            if (surface)
                updateSurface();
        }

        virtual void setPosition(Physical x, Physical y) noexcept
        {
            this->x = x;
            this->y = y;
            if (visual)
            {
                visual->SetOffsetX((float)x);
                visual->SetOffsetY((float)y);
            }
        }

        // this method will be called when animating positions to set final value.
        void updatePosition(Physical x, Physical y) noexcept
        {
            this->x = x;
            this->y = y;
        }

        Physical getX() { return x; }
        Physical getY() { return y; }

        virtual void setSize(Physical width, Physical height) noexcept
        {
            this->width = width;
            this->height = height;
            if (surface)
            {
                recreateSurface();
                visual->SetContent(surface);
            }
        }

        // you can also pass nullptr to newParentLayer to remove this layer from parent.
        // or call removeFromParent().
        virtual void setParentLayer(Layer* newParentLayer) noexcept
        {
            if (parentLayer)
            {
                IDCompositionVisual2* parentVisual = parentLayer->getVisual();
                if (parentVisual) // parent is created
                {
                    if(visual)
                        parentVisual->RemoveVisual(visual);
                }
            }
            else // no parent layer. so, rootVisual is the parent.
            {
                if (rootVisual) // already created & attached to root
                {
                    if (visual)
                        rootVisual->RemoveVisual(visual);
                }
            }

            parentLayer = newParentLayer;
            if (parentLayer)
            {
                IDCompositionVisual2* parentVisual = parentLayer->getVisual();
                if (parentVisual) // parent is already created
                {
                    if (visual)
                        parentVisual->AddVisual(visual, FALSE, nullptr);
                }
            }
        }

        virtual bool createGPUResources(IDCompositionDesktopDevice* compositionDevice, 
            ID2D1DeviceContext* d2dDeviceContext, IDCompositionVisual2* rootVisual) noexcept
        {
            // we don't increase ref count. :-)
            this->compositionDevice = compositionDevice;
            this->d2dDeviceContext = d2dDeviceContext;
            this->rootVisual = rootVisual;

            compositionDevice->CreateVisual(&visual);

            recreateSurface();
            visual->SetContent(surface);

            visual->SetOffsetX((float)x);
            visual->SetOffsetY((float)y);
            
            if (visible)
            {
                if (parentLayer)
                {
                    IDCompositionVisual2* parentVisual = parentLayer->getVisual();
                    if (parentVisual)
                        parentVisual->AddVisual(visual, FALSE, nullptr);
                }
                else
                {
                    rootVisual->AddVisual(visual, FALSE, nullptr);
                }
            }
            
            return true;
        }

        virtual void updateSurface() noexcept
        {
            ID2D1DeviceContext* dc;
            POINT offset = {};
            surface->BeginDraw(nullptr, // Entire surface
                __uuidof(ID2D1DeviceContext),
                (void**)&dc,
                &offset);

            dc->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));
            dc->Clear(backgroundColor);

            surface->EndDraw();
            dc->Release();
        }

        virtual void onDPIChange(int oldDPI, int newDPI) noexcept
        {
            x = ::MulDiv(x, newDPI, oldDPI);
            y = ::MulDiv(y, newDPI, oldDPI);
            width = ::MulDiv(width, newDPI, oldDPI);
            height = ::MulDiv(height, newDPI, oldDPI);

            recreateSurface();
            visual->SetContent(surface);
            visual->SetOffsetX((float)x);
            visual->SetOffsetY((float)y);
        }

        virtual void releaseGPUResources() noexcept
        {
            if (surface)
            {
                surface->Release();
                surface = nullptr;
            }

            if (visual)
            {
                visual->Release();
                visual = nullptr;
            }

            d2dDeviceContext = nullptr;
            compositionDevice = nullptr;
            rootVisual = nullptr;
        }
    };

    class AnimatorBase
    {
    protected:
        Layer* layer = nullptr;
        IDCompositionDesktopDevice* compositionDevice = nullptr;
        IUIAnimationManager2* animationManager = nullptr;
        IUIAnimationTransitionLibrary2* transitionLibrary = nullptr;
    public:
        AnimatorBase() noexcept {}
        virtual ~AnimatorBase() noexcept {}

        // call at constructor of BaseWindow
        void setAnimationManager(IUIAnimationManager2* animationManager, IUIAnimationTransitionLibrary2* transitionLibrary) noexcept
        {
            this->animationManager = animationManager;
            this->transitionLibrary = transitionLibrary;
        }

        // call at BaseWindow::createGPUResources method.
        void setCompositionDevice(IDCompositionDesktopDevice* compositionDevice) noexcept
        {
            this->compositionDevice = compositionDevice;
        }

        virtual void setLayer(Layer* layer) noexcept
        {
            this->layer = layer;
        }
    };

    class XPosAnimator : public AnimatorBase
    {
    protected:
        IUIAnimationVariable2* animationVarX = nullptr;
        
    public:
        XPosAnimator() noexcept {}
        ~XPosAnimator() noexcept
        {
            if (animationVarX)
                animationVarX->Release();
        }

        // supports to change layer to another after setting it to one.
        virtual void setLayer(Layer* layer) noexcept override
        {
            __super::setLayer(layer);

            if (animationVarX)
            {
                animationVarX->Release();
                animationVarX = nullptr;
            }
        }

        // when you call this method, layer x position value is assigned endX before ending the animation.
        void start(Physical endX, DOUBLE accelerationRatio = 0.1, DOUBLE decelerationRatio=0.9, DOUBLE duration=0.3) noexcept
        {
            if ((layer == nullptr) || (animationManager == nullptr) || 
                (transitionLibrary == nullptr) || (compositionDevice == nullptr))
                return;

            IDCompositionVisual2* layerVisual = layer->getVisual();
            if (layerVisual == nullptr)
                return;

            // using animation manager to generate animation segments
            IUIAnimationStoryboard2* pStoryboard = nullptr; 
            animationManager->CreateStoryboard(&pStoryboard);

            // Synchronizing WAM and DirectComposition time such that when WAM Update is called, 
            // the value reflects the DirectComposition value at the given time.
            DCOMPOSITION_FRAME_STATISTICS frameStatistics = { 0 };
            compositionDevice->GetFrameStatistics(&frameStatistics);
            UI_ANIMATION_SECONDS nextEstimatedFrameTime = static_cast<double>(frameStatistics.nextEstimatedFrameTime.QuadPart) / static_cast<double>(frameStatistics.timeFrequency.QuadPart);
            //Upating the WAM time 
            animationManager->Update(nextEstimatedFrameTime);

            // using old animation variable if exists. so the new animation will continue where current animation last stopped.
            if (animationVarX == nullptr)
            {
                animationManager->CreateAnimationVariable((DOUBLE)layer->getX(), &animationVarX);
            }

            IUIAnimationTransition2* pTransition = nullptr;
            transitionLibrary->CreateAccelerateDecelerateTransition(
                duration, (DOUBLE)endX, 0.1, 0.9, &pTransition);

            pStoryboard->AddTransition(animationVarX, pTransition);

            //schedule the storyboard for play at the next estimate vblank
            pStoryboard->Schedule(nextEstimatedFrameTime);

            IDCompositionAnimation* compositionAnimation = nullptr;
            compositionDevice->CreateAnimation(&compositionAnimation);

            animationVarX->GetCurve(compositionAnimation); // add segments to compositionAnimation.

            layerVisual->SetOffsetX(compositionAnimation);
            compositionDevice->Commit(); // start animation

            layer->updatePosition(endX, layer->getY()); // update layer position x value to final x value.

            compositionAnimation->Release();
            pTransition->Release();
            pStoryboard->Release();
        }

    };

    // A window which uses visual layers instead of redirection surface of the dwm.
    // after you modify visual surface, call BaseWindow::commit method.
    // you cannot have ws_child controls within this window.
    // no drop shadows or non rectangular top level windows. because we cannot clip child visuals to parent shape.
    // override createGPUResources, releaseGPUResources, updateAllSurfaces and onDPIChange methods in your subclass and
    // -create your own visual layers!
    class BaseWindow : public KWindow
    {
    protected:
        ID3D11Device* d3d11Device = nullptr;
        ID3D11DeviceContext* d3d11DeviceContext = nullptr;
        IDXGIDevice* dxgiDevice = nullptr;
        ID2D1Device* d2dDevice = nullptr;
        ID2D1DeviceContext* d2dDeviceContext = nullptr; // use this to create brushes, bitmaps etc...
        IDCompositionDesktopDevice* compositionDevice = nullptr;
        IDCompositionTarget* compositionTarget = nullptr;
        IDCompositionVisual2* rootVisual = nullptr;
        IDCompositionSurface* rootSurface = nullptr;

        IUIAnimationManager2* animationManager = nullptr;
        IUIAnimationTransitionLibrary2* transitionLibrary = nullptr;

        // override this method in subclass then release your own objects!
        // make sure to call __super::releaseGPUResources() after it.
        // this method may called multiple times at runtime if gpu is disabled.
        virtual void releaseGPUResources() noexcept
        {
            if (rootSurface)
            {
                rootSurface->Release();
                rootSurface = nullptr;
            }

            if (rootVisual)
            {
                rootVisual->Release();
                rootVisual = nullptr;
            }

            if (compositionTarget)
            {
                compositionTarget->Release();
                compositionTarget = nullptr;
            }

            if (d2dDeviceContext)
            {
                d2dDeviceContext->Release();
                d2dDeviceContext = nullptr;
            }

            if (compositionDevice)
            {
                compositionDevice->Release();
                compositionDevice = nullptr;
            }

            if (d2dDevice)
            {
                d2dDevice->Release();
                d2dDevice = nullptr;
            }

            if (dxgiDevice)
            {
                dxgiDevice->Release();
                dxgiDevice = nullptr;
            }

            if (d3d11DeviceContext)
            {
                d3d11DeviceContext->Release();
                d3d11DeviceContext = nullptr;
            }

            if (d3d11Device)
            {
                d3d11Device->Release();
                d3d11Device = nullptr;
            }
        }

        // override this method in subclass then create your own visuals,surfaces & hierarchies!
        // make sure to call __super::createGPUResources() as first line.
        // this method may called multiple times at runtime if gpu is disabled.
        // so, save position, sizes of visuals and surfaces then restore them at here also.
        virtual bool createGPUResources(HWND hwnd) noexcept
        {
            D3D_FEATURE_LEVEL featureLevelSupported;
            HRESULT hr = ::D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                NULL,
                D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                NULL,
                0,
                D3D11_SDK_VERSION,
                &d3d11Device,
                &featureLevelSupported,
                &d3d11DeviceContext);

            if (FAILED(hr))
            {
                // try warp
                hr = ::D3D11CreateDevice(
                    nullptr,
                    D3D_DRIVER_TYPE_WARP,
                    NULL,
                    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                    NULL,
                    0,
                    D3D11_SDK_VERSION,
                    &d3d11Device,
                    &featureLevelSupported,
                    &d3d11DeviceContext);
            }

            if (FAILED(hr))
            {
                ::MessageBoxW(0, L"D3D11Device create failed!", L"error", MB_ICONERROR);
                return false;
            }

            d3d11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

            hr = ::D2D1CreateDevice(dxgiDevice, nullptr, &d2dDevice);
            if (FAILED(hr))
            {
                ::MessageBoxW(0, L"D2D1CreateDevice failed!", L"error", MB_ICONERROR);
                return false;
            }

            hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext);
            if (FAILED(hr))
            {
                ::MessageBoxW(0, L"D2D CreateDeviceContext failed!", L"error", MB_ICONERROR);
                return false;
            }

            // we are using DirectComposition V2 (win8.1), so we need to use IDCompositionDesktopDevice instead of IDCompositionDevice.
            // using d2dDevice will allow surface->BeginDraw to create ID2D1DeviceContext.
            hr = ::DCompositionCreateDevice2(d2dDevice, __uuidof(IDCompositionDesktopDevice),
                (void**)&compositionDevice);
            if (FAILED(hr))
            {
                ::MessageBoxW(0, L"DCompositionCreateDevice2 failed!", L"error", MB_ICONERROR);
                return false;
            }

            hr = compositionDevice->CreateTargetForHwnd(hwnd,
                TRUE, // visual tree should be displayed on top of the children of the window
                &compositionTarget);
            if (FAILED(hr))
            {
                ::MessageBoxW(0, L"CreateTargetForHwnd failed!", L"error", MB_ICONERROR);
                return false;
            }

            compositionDevice->CreateVisual(&rootVisual);

            RECT rect;
            ::GetWindowRect(compHWND, &rect);
            const int wndWidth = rect.right - rect.left;
            const int wndHeight = rect.bottom - rect.top;

            compositionDevice->CreateSurface(wndWidth,
                wndHeight,
                DXGI_FORMAT_B8G8R8A8_UNORM,
                DXGI_ALPHA_MODE_PREMULTIPLIED,
                &rootSurface);

            rootVisual->SetContent(rootSurface);
            compositionTarget->SetRoot(rootVisual);
            return true;
        }

        // override this method in subclass then draw on your all surfaces to match their current state.
        // this method will be called at startup, dpi change and recovering from gpu disable.
        // make sure to call __super::updateAllSurfaces() as first line.
        // after this call there will be a commit().
        virtual void updateAllSurfaces() noexcept
        {
            ID2D1DeviceContext* dc;
            POINT offset = {};
            rootSurface->BeginDraw(nullptr, // Entire surface
                __uuidof(ID2D1DeviceContext),
                (void**)&dc,
                &offset);

            dc->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));
            dc->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray));

            rootSurface->EndDraw();
            dc->Release();
        }

        // override this method in subclass then resize your surfaces and reposition visuals!
        // after this call there will be an updateAllSurfaces() and commit().
        // make sure to call __super::onDPIChange() as first line.
        virtual void onDPIChange(int oldDPI, int newDPI) noexcept
        {
            // window size already updated to new dpi at this moment.
            resizeRootSurface();
        }

        void resizeRootSurface() noexcept
        {
            if (compositionDevice == nullptr)
                return;

            RECT rect;
            ::GetWindowRect(compHWND, &rect);

            const int wndWidth = rect.right - rect.left;
            const int wndHeight = rect.bottom - rect.top;

            if (rootSurface)
            {
                rootSurface->Release();
                rootSurface = nullptr;
            }

            compositionDevice->CreateSurface(wndWidth,
                wndHeight,
                DXGI_FORMAT_B8G8R8A8_UNORM,
                DXGI_ALPHA_MODE_PREMULTIPLIED,
                &rootSurface);
            rootVisual->SetContent(rootSurface);
        }

        LRESULT onDPIChangeMsg(WPARAM wParam, LPARAM lParam) noexcept
        {
            int oldDPI = compFontRef.getCurrentDPI();
            int newDPI = HIWORD(wParam);

            LRESULT result = __super::windowProc(compHWND, WM_DPICHANGED, wParam, lParam);
            onDPIChange(oldDPI, newDPI);
            updateAllSurfaces();
            commit();
            return result;
        }

        LRESULT onNCCalcSizeMsg(WPARAM wParam, LPARAM lParam) noexcept
        {
            // WS_CAPTION is needed to display minimize animation.
            // but to avoid displaying titlebar, we return 0 at here.
            if (compDwStyle & WS_CAPTION)
                return 0; // Removes non-client area (caption, borders)

            return __super::windowProc(compHWND, WM_NCCALCSIZE, wParam, lParam);
        }

        LRESULT onPaintMsg(WPARAM wParam, LPARAM lParam) noexcept
        {
            ::ValidateRect(compHWND, NULL);

            // if dcompostion detects a lost device, it also sends the WM_PAINT.
            // commit() will detect gpu loss and recreate gpu resources!
            if (compositionDevice != nullptr)
                commit();

            return 0;
        }

        LRESULT onEraseBackgroundMsg(WPARAM wParam, LPARAM lParam) noexcept
        {
            return 1; // avoids flickering
        }

    public:
        BaseWindow() noexcept
        {
            compText = L"BaseWindow";
            compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
            compDwExStyle = WS_EX_NOREDIRECTIONBITMAP | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
            compLWidth = 720;
            compLHeight = 480;

            ::CoCreateInstance(
                CLSID_UIAnimationManager2,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_IUIAnimationManager2,
                reinterpret_cast<LPVOID*>(&animationManager));

            ::CoCreateInstance(
                CLSID_UIAnimationTransitionLibrary2,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_IUIAnimationTransitionLibrary2,
                reinterpret_cast<LPVOID*>(&transitionLibrary));
        }

        // combine all visuals and display. if gpu loss detected, it will recreate gpu resources.
        virtual void commit() noexcept
        {
            if (compositionDevice == nullptr)
                return;

            HRESULT hr = compositionDevice->Commit();
            if (SUCCEEDED(hr))
                return;

            releaseGPUResources();
            if (!createGPUResources(compHWND))
            {
                releaseGPUResources();
                ::MessageBoxW(0, L"createGPUResources failed!", L"error", MB_ICONERROR);
                ::ExitProcess(1);
            }

            updateAllSurfaces();
            compositionDevice->Commit();
        }

        virtual bool create(bool requireInitialMessages = false)  noexcept override
        {
            if (enableDPIUnawareMode)
                this->applyDPIUnawareModeToThread();

            // if the window has WS_CAPTION style and if you want to hide it, then requireInitialMessages must be true.
            // it will allow us to catch WM_NCCALCSIZE message and hide the title bar.
            requireInitialMessages = (compDwStyle & WS_CAPTION) == WS_CAPTION;

            if (::RegisterClassExW(&wc))
            {
                isRegistered = true;
                KGUIProc::createComponentFor96DPI(this, requireInitialMessages, compPX, compPY);
                if (compHWND)
                {
                    if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode))
                    {
                        // if the default monitor is scaled then we won't get dpi change message.
                        // so we do manual positioning/scaling for the window.
                        // if the SetWindowPos causes window to position over scaled second monitor, then os will send dpi change message.
                        const int dpi_1 = getDPI();
                        ::SetWindowPos(compHWND,
                            NULL,
                            compPX,
                            compPY,
                            KDPIUtility::toPhysical(compLWidth, dpi_1),
                            KDPIUtility::toPhysical(compLHeight, dpi_1),
                            SWP_NOZORDER | SWP_NOACTIVATE);
                    }

                    ::EnableWindow(compHWND, compEnabled ? TRUE : FALSE);

                    if (createGPUResources(compHWND))
                    {
                        updateAllSurfaces();
                        commit();

                        if (enableDPIUnawareMode)
                            this->restoreDPIModeOfThread();

                        afterCreated();
                        return true;
                    }
                    else
                    {
                        releaseGPUResources();
                        ::ExitProcess(1);
                    }
                }
            }

            if (enableDPIUnawareMode)
                this->restoreDPIModeOfThread();

            return false;
        }

        virtual void setSize(Logical compWidth, Logical compHeight)  noexcept override
        {
            __super::setSize(compWidth, compHeight);
            if (compositionDevice)
            {
                resizeRootSurface();
                updateAllSurfaces();
                commit();
            }
        }

        virtual void onDestroy()  noexcept override
        {
            releaseGPUResources();
            __super::onDestroy();
        }

        virtual ~BaseWindow() noexcept
        {
            if (transitionLibrary)
                transitionLibrary->Release();

            if (animationManager)
                animationManager->Release();
        }

        BEGIN_KMSG_HANDLER
            ON_KMSG(WM_PAINT, onPaintMsg)
            ON_KMSG(WM_ERASEBKGND, onEraseBackgroundMsg)
            ON_KMSG(WM_DPICHANGED, onDPIChangeMsg)
            ON_KMSG(WM_NCCALCSIZE, onNCCalcSizeMsg)
        END_KMSG_HANDLER
    };


    class CompExampleWindow : public BaseWindow
    {
    protected:
        Layer sideBarLayer;
        Layer sideBarRect;
        XPosAnimator sideBarAnimator;

        virtual void releaseGPUResources() noexcept override
        {
            sideBarRect.releaseGPUResources();
            sideBarLayer.releaseGPUResources();
            __super::releaseGPUResources();
        }

        virtual bool createGPUResources(HWND hwnd) noexcept override
        {
            if (!__super::createGPUResources(hwnd))
                return false;

            sideBarAnimator.setCompositionDevice(compositionDevice);

            // parent layer must be created before the child layer.
            sideBarLayer.createGPUResources(compositionDevice, d2dDeviceContext, rootVisual);
            return sideBarRect.createGPUResources(compositionDevice, d2dDeviceContext, rootVisual);
        }

        virtual void updateAllSurfaces() noexcept override
        {
            __super::updateAllSurfaces();
            sideBarLayer.updateSurface();
            sideBarRect.updateSurface();
        }

        virtual void onDPIChange(int oldDPI, int newDPI) noexcept override
        {
            __super::onDPIChange(oldDPI, newDPI);
            sideBarLayer.onDPIChange(oldDPI, newDPI);
            sideBarRect.onDPIChange(oldDPI, newDPI);
        }

        LRESULT onLButtonDownMsg(WPARAM wParam, LPARAM lParam) noexcept
        {
            //sideBarRect.setVisible(!sideBarRect.isVisible());
            if(sideBarLayer.getX() == -50)
                sideBarAnimator.start(0);
            else if (sideBarLayer.getX() == 0)
                sideBarAnimator.start(-50);

            return 0;
        }

    public:
        CompExampleWindow() noexcept
        {
            sideBarLayer.setPosition(-50, 0);
            sideBarLayer.setSize(50, compLHeight);
            sideBarLayer.setBackgroundColor(D2D1::ColorF::DarkBlue);

            sideBarRect.setPosition(5, 5);
            sideBarRect.setSize(40, 40);
            sideBarRect.setBackgroundColor(D2D1::ColorF::Yellow);
            sideBarRect.setParentLayer(&sideBarLayer);

            sideBarAnimator.setAnimationManager(animationManager, transitionLibrary);
            sideBarAnimator.setLayer(&sideBarLayer);
        }

        BEGIN_KMSG_HANDLER
            ON_KMSG(WM_LBUTTONDOWN, onLButtonDownMsg)
        END_KMSG_HANDLER

    };
}

