
// Shows how to use KSettingsReader class and how to resize a control manually on dpi change.

#include "rfc/rfc.h"

// use your own file format ID! (4 chars)
#define MY_FORMAT_ID KFORMAT_ID('WVR1')

class MyWindow : public KOverlappedWindow, public KDPIChangeListener
{
protected:
	KTextArea textArea;
	KString settingsFile;
	KFont font;

public:
	MyWindow()
	{
		setDPIChangeListener(this);
		setText(L"My Window");
		create();

		font.load(L"Segoe UI", 16, false, false, false, true, this->GetDPI());
		textArea.SetPosition(0, 0);
		textArea.SetFont(font);

		addComponent(textArea);

		wchar_t pathBuffer[MAX_PATH];
		KDirectory::getRoamingFolder(pathBuffer);

		KString appDataDir =  KString(pathBuffer, KStringBehaviour::DO_NOT_FREE) + L"\\TestRFCApp1";
		KDirectory::createDir(appDataDir);

		settingsFile = appDataDir + L"\\settings.test";

		// load settings
		KSettingsReader settingsReader;
		if (settingsReader.openFile(settingsFile, MY_FORMAT_ID))
		{
			textArea.setText(settingsReader.readString());

			int x = settingsReader.readInt();
			int y = settingsReader.readInt();
			if(!KWindow::isOffScreen(x, y)) // position is not on turned off monitor.
				setPosition(x, y);	

			int width = settingsReader.readInt();
			int height = settingsReader.readInt();
			setSize(width, height);
		}
	}

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if the window and controls are in different dpi scale. (use KDPIChangeListener)
	void onResized() override
	{
		if (this->getDPI() == textArea.getDPI())
		{
			int width, height;
			this->getClientAreaSize(&width, &height);
			textArea.setSize(width, height);
		}
	}

	// this method will be called after the dpi change. (KDPIChangeListener)
	void onDPIChange(HWND hwnd, int newDPI) override
	{
		// on dpi change, the textArea will be automatically resized by the framework. 
		// But we want the size of textArea to be exact size of the client area!
		int width, height;
		this->getClientAreaSize(&width, &height);
		textArea.setSize(width, height);
	}

	void onClose() override
	{
		KSettingsWriter settingsWriter;

		// save settings
		if (settingsWriter.openFile(settingsFile, MY_FORMAT_ID))
		{
			settingsWriter.writeString(textArea.getText());
			settingsWriter.writeInt(this->compX);
			settingsWriter.writeInt(this->compY);
			settingsWriter.writeInt(this->compWidth);
			settingsWriter.writeInt(this->compHeight);
		}

		KOverlappedWindow::onClose();
	}
};

class MyApplication : public KApplication
{
public:

	int main(wchar_t** argv, int argc)
	{
		MyWindow wnd;
		wnd.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);