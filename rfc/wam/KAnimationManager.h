
#pragma once

#include "../core/CoreModule.h"
#include "../com/COMModule.h"
#include "../containers/ContainersModule.h"

#include <uianimation.h>

class KAnimationUpdateListener
{
public:
	// invalidate only if your storyboard is running.
	virtual void onAnimationUpdate() noexcept = 0;
};

class KAnimationEventListener
{
public:
	// invalidate only if your storyboard is running.
	virtual void onAnimationStart() noexcept = 0;
};

// Animation variable represents the characteristic to be animated.
// A transition describes how the value of that animation variable changes as animation occurs.
// For example, a visual element might have an animation variable that specifies its opacity, 
// and a user action might generate a transition that takes that opacity from a value of 50 to 100, 
// representing an animation from semi-transparent to fully opaque.
// A storyboard is a set of transitions applied to one or more animation variables over time. 
// An application displays animations by constructing and playing storyboards and then drawing 
// -sequences of discrete frames as the values of animation variables change over time.
// An animation variable typically has the same lifetime as the visual element it is to animate. 
// The initial value of an animation variable is specified when the variable is created. 
// Thereafter, its value cannot be changed directly; it must be updated through the animation manager.
// OS support : win7 or higher
// see : https://learn.microsoft.com/en-us/windows/win32/uianimation/storyboard-construction
class KAnimationManager
{
private:
	class AnimationEventHandler : public KComObject<AnimationEventHandler, IUIAnimationManagerEventHandler2>
	{
		KPointerList<KAnimationEventListener*, 8, false> eventListeners;
	public:
		IFACEMETHODIMP OnManagerStatusChanged(UI_ANIMATION_MANAGER_STATUS newStatus, UI_ANIMATION_MANAGER_STATUS previousStatus) noexcept
		{
			HRESULT hr = S_OK;

			if (newStatus == UI_ANIMATION_MANAGER_BUSY)
			{
				eventListeners.forEach([](KAnimationEventListener* listener) {
					listener->onAnimationStart();
					});
				return S_OK;
			}
			return hr;
		}
		void addListener(KAnimationEventListener* listener) noexcept
		{
			eventListeners.add(listener);
		}

		void removeListener(KAnimationEventListener* listener) noexcept
		{
			eventListeners.remove(listener);
		}
	};

	KComPtr<IUIAnimationManager2> animationManager;
	KComPtr<IUIAnimationTransitionLibrary2> transitionLibrary;
	KComPtr<IUIAnimationTimer> animationTimer;
	AnimationEventHandler eventHandler;

	KAnimationManager() noexcept
	{
		HRESULT hr;
		hr = ::CoCreateInstance(
			CLSID_UIAnimationManager2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IUIAnimationManager2,
			animationManager);

		hr = ::CoCreateInstance(
			CLSID_UIAnimationTransitionLibrary2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IUIAnimationTransitionLibrary2,
			transitionLibrary);

		hr = ::CoCreateInstance(
			CLSID_UIAnimationTimer,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IUIAnimationTimer,
			animationTimer
		);

		animationManager->SetManagerEventHandler(&eventHandler);
	}
public:

	static KAnimationManager* getInstance() noexcept
	{
		static KAnimationManager* _instance = nullptr;
		if (_instance == nullptr)
			_instance = new KAnimationManager();

		return _instance;
	}

	void shutdown() noexcept
	{
		animationManager->Shutdown();
	}

	void update() noexcept
	{
		UI_ANIMATION_SECONDS secondsNow;
		animationTimer->GetTime(&secondsNow);
		animationManager->Update(secondsNow);
	}

	void addEventListener(KAnimationEventListener* listener) noexcept
	{
		eventHandler.addListener(listener);
	}

	void removeEventListener(KAnimationEventListener* listener) noexcept
	{
		eventHandler.removeListener(listener);
	}

	KComPtr<IUIAnimationStoryboard2> createStoryboard() noexcept
	{
		KComPtr<IUIAnimationStoryboard2> pStoryboard;
		animationManager->CreateStoryboard(pStoryboard.getAddressOf());
		return pStoryboard;
	}

	void scheduleStoryboard(IUIAnimationStoryboard2* storyboard) noexcept
	{
		UI_ANIMATION_SECONDS secondsNow;
		animationTimer->GetTime(&secondsNow);

		storyboard->Schedule(secondsNow);
	}

	KComPtr<IUIAnimationVariable2> createVariable(double initialValue = 0.0) noexcept
	{
		KComPtr<IUIAnimationVariable2> pVariable;
		animationManager->CreateAnimationVariable(initialValue, pVariable.getAddressOf());
		return pVariable;
	}

	KComPtr<IUIAnimationTransition2> createAccelerateDecelerateTransition(int endX, DOUBLE accelerationRatio = 0.1,
		DOUBLE decelerationRatio = 0.9, DOUBLE duration = 0.3) noexcept
	{
		KComPtr<IUIAnimationTransition2> pTransition;
		transitionLibrary->CreateAccelerateDecelerateTransition(
			duration, (DOUBLE)endX, accelerationRatio, decelerationRatio, pTransition.getAddressOf());
		return pTransition;
	}

	static bool isStoryboardRunning(IUIAnimationStoryboard2* storyboard) noexcept
	{
		UI_ANIMATION_STORYBOARD_STATUS status;
		storyboard->GetStatus(&status);
		return (status == UI_ANIMATION_STORYBOARD_PLAYING);
	}

	~KAnimationManager() noexcept
	{
		// this will unlink eventHandler from animationManager. 
		// so the destroying order of animationTimer & eventHandler does not matter.
		animationManager->SetManagerEventHandler(NULL);
	}
};