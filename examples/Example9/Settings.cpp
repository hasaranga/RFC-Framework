
#include "../../amalgamated/rfc_amalgamated.h"


// use your own file format ID! (4 chars)
#define MY_FORMAT_ID KFORMAT_ID('WVR1')

class MyWindow : public KOverlappedWindow
{
protected:
	KTextArea textArea;
	KString settingsFile;

public:
	MyWindow()
	{
		this->CreateComponent();

		textArea.SetPosition(0, 0);
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
			this->SetPosition(x, y);

			int width = settingsReader.ReadInt();
			int height = settingsReader.ReadInt();
			this->SetSize(width, height);
		}

	}

	void OnResized()
	{
		int width, height;
		this->GetClientAreaSize(&width, &height);

		textArea.SetSize(width, height);
	}

	void OnClose()
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

	int Main(KString **argv, int argc)
	{
		MyWindow wnd;
		wnd.SetVisible(true);

		DoMessagePump();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication);