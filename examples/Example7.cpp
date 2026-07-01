
// Shows how to use KSettingsReader class and how to resize a control manually on dpi change.

#include "rfc/rfc.h"

// use your own file format ID! (4 chars)
#define MY_FORMAT_ID KFORMAT_ID('WVR1')

class MyWindow : public KOverlappedWindow
{
protected:
	KTextArea textArea;
	KString settingsFile;

public:
	MyWindow() noexcept
	{
		setText(L"My Window");
		create();

		onDPIChange = [this](KWindow*, int newDPI) {
			// on dpi change, the textArea will be automatically resized by the framework. 
			// But we want the size of textArea to be exact size of the client area!
			int width, height;
			this->getClientAreaSize(width, height);
			textArea.setSize(width, height);
		};

		textArea.setPosition(0, 0);
		textArea.setFontType(KFontType(L"Segoe UI", 18));

		addComponent(textArea);

		wchar_t pathBuffer[MAX_PATH];
		KDirectory::getRoamingFolder(pathBuffer);

		KString appDataDir = KString(pathBuffer, KStringBehaviour::DO_NOT_FREE) + L"\\TestRFCApp1";
		KDirectory::createDir(appDataDir);

		settingsFile = appDataDir + L"\\settings.test";

		// load settings
		KSettingsReader settingsReader;
		if (settingsReader.openFile(settingsFile, MY_FORMAT_ID))
		{
			textArea.setText(settingsReader.readString());

			const Physical x = settingsReader.readInt(); // physical value
			const Physical y = settingsReader.readInt(); // physical value

			const Logical width = settingsReader.readInt(); // logical value
			const Logical height = settingsReader.readInt(); // logical value
			setSize(width, height);

			// check if position is not on offscreen / turned off monitor.
			if (KWindow::isOffScreen(x, y))
				centerScreen();
			else
				setPositionPhysical(x, y);
		}
	}

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if resizingForDPIChange is true. (to detect dpi changes use onDPIChange event)
	void onResized(bool resizingForDPIChange) noexcept override
	{
		if (!resizingForDPIChange)
		{
			Logical width, height;
			this->getClientAreaSize(width, height);
			textArea.setSize(width, height);
		}
	}

	void onClose() noexcept override
	{
		KSettingsWriter settingsWriter;

		// save settings
		if (settingsWriter.openFile(settingsFile, MY_FORMAT_ID))
		{
			settingsWriter.writeString(textArea.getText());
			Physical x, y;
			getPositionPhysical(x, y);
			settingsWriter.writeInt(x);
			settingsWriter.writeInt(y);

			Logical w, h;
			getNormalSize(w, h);
			settingsWriter.writeInt(w);
			settingsWriter.writeInt(h);
		}

		KOverlappedWindow::onClose();
	}
};

class MyApplication : public KApplication
{
public:

	int main(wchar_t** argv, int argc) noexcept
	{
		MyWindow wnd;
		wnd.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);