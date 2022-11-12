
// Shows how to use KSettingsReader class and how to resize a control manually on dpi change.

#include "rfc/rfc.h"

// use your own file format ID! (4 chars)
#define MY_FORMAT_ID KFORMAT_ID('WVR1')

class MyWindow : public KOverlappedWindow, public KDPIChangeListener
{
protected:
	KTextArea textArea;
	KString settingsFile;
	KScopedClassPointer<KFont> font;

public:
	MyWindow()
	{
		this->SetDPIChangeListener(this);
		this->SetText(L"My Window");
		this->Create();

		font = new KFont(L"Segoe UI", 16, false, false, false, true, this->GetDPI());
		textArea.SetPosition(0, 0);
		textArea.SetFont(font);

		this->AddComponent(&textArea);

		KString appDataDir = KDirectory::GetApplicationDataDir() + L"\\TestRFCApp1";
		KDirectory::CreateDir(appDataDir);

		settingsFile = appDataDir + L"\\settings.test";

		// load settings
		KSettingsReader settingsReader;
		if (settingsReader.OpenFile(settingsFile, MY_FORMAT_ID))
		{
			textArea.SetText(settingsReader.ReadString());

			int x = settingsReader.ReadInt();
			int y = settingsReader.ReadInt();
			if(!KWindow::IsOffScreen(x, y)) // position is not on turned off monitor.
				this->SetPosition(x, y);	

			int width = settingsReader.ReadInt();
			int height = settingsReader.ReadInt();
			this->SetSize(width, height);
		}
	}

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if the window and controls are in different dpi scale. (use KDPIChangeListener)
	void OnResized() override
	{
		if (this->GetDPI() == textArea.GetDPI())
		{
			int width, height;
			this->GetClientAreaSize(&width, &height);
			textArea.SetSize(width, height);
		}
	}

	// this method will be called after the dpi change. (KDPIChangeListener)
	void OnDPIChange(HWND hwnd, int newDPI) override
	{
		// on dpi change, the textArea will be automatically resized by the framework. 
		// But we want the size of textArea to be exact size of the client area!
		int width, height;
		this->GetClientAreaSize(&width, &height);
		textArea.SetSize(width, height);
	}

	void OnClose() override
	{
		KSettingsWriter settingsWriter;

		// save settings
		if (settingsWriter.OpenFile(settingsFile, MY_FORMAT_ID))
		{
			settingsWriter.WriteString(textArea.GetText());
			settingsWriter.WriteInt(this->compX);
			settingsWriter.WriteInt(this->compY);
			settingsWriter.WriteInt(this->compWidth);
			settingsWriter.WriteInt(this->compHeight);
		}

		KOverlappedWindow::OnClose();
	}
};

class MyApplication : public KApplication
{
public:

	int Main(KString** argv, int argc)
	{
		MyWindow wnd;
		wnd.SetVisible(true);

		KApplication::MessageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);