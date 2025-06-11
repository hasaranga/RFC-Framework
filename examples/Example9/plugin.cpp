
// Winamp DSP Plug-In Example

#include "rfc/rfc.h"
#include"resource.h"
#include "dsp.h" 


#define MY_FORMAT_ID KFORMAT_ID('dsp1')

class PluginWindow : public KHotPluggedDialog
{
protected:
	KCheckBox chkEnableProcessing;
	KString settingsFile;
	volatile bool processAudioFlag;

public:
	void onHotPlug()
	{
		KHotPluggedDialog::onHotPlug();

		processAudioFlag = false;

		chkEnableProcessing.hotPlugInto(::GetDlgItem(compHWND, IDC_CHECK1));
		chkEnableProcessing.setCheckedState(false);
		chkEnableProcessing.onClick = [this](KButton* button){
			processAudioFlag = chkEnableProcessing.IsChecked();
		};

		wchar_t buffer[MAX_PATH];
		KDirectory::getRoamingFolder(buffer);

		KString appDataDir = KString(buffer, KStringBehaviour::DO_NOT_FREE) + L"\\TestWinampDSP1";
		KDirectory::createDir(appDataDir);

		settingsFile = appDataDir + L"\\settings.conf";

		// load settings
		KSettingsReader settingsReader;
		if (settingsReader.openFile(settingsFile, MY_FORMAT_ID))
		{
			processAudioFlag = settingsReader.readBool();
			chkEnableProcessing.setCheckedState(processAudioFlag);

			int x = settingsReader.readInt();
			int y = settingsReader.readInt();
			if(!KWindow::isOffScreen(x, y))
				setPosition(x, y);

			bool visible = settingsReader.readBool();
			setVisible(visible);
		}

	}

	void saveSettings()
	{
		KSettingsWriter settingsWriter;

		// save settings
		if (settingsWriter.openFile(settingsFile, MY_FORMAT_ID))
		{
			settingsWriter.writeBool(processAudioFlag);
			settingsWriter.writeInt(getX());
			settingsWriter.writeInt(getY());
			settingsWriter.writeBool(isVisible());
		}
	}

	int processAudio(short int *samples, int numsamples, int bps, int nch, int srate)
	{
		if (processAudioFlag)
		{
			if ((bps == 16) && (nch == 2) && (numsamples > 0))
			{
				short *a = samples;
				int x = numsamples;
				while (x--)
				{
					a[0] = a[0] - a[1];
					a[1] = a[0];

					a += 2;
				}
			}
		}
		return numsamples;
	}

	void onClose()
	{
		setVisible(false);
	}
};

PluginWindow *pluginWindow;

int init(struct winampDSPModule *this_mod)
{
	RFCDllInit();

	pluginWindow = new PluginWindow();
	KGUIProc::hotPlugAndCreateDialogBox(IDD_DIALOG1, this_mod->hwndParent, pluginWindow);

	return 0;
}

void config(struct winampDSPModule *this_mod)
{
	pluginWindow->setVisible(true);
	pluginWindow->bringToFront();
}

void quit(struct winampDSPModule *this_mod)
{
	pluginWindow->saveSettings();
	pluginWindow->destroy();
	delete pluginWindow;

	RFCDllFree();
}

int modify_samples(struct winampDSPModule *this_mod, short int *samples, int numsamples, int bps, int nch, int srate)
{
	return pluginWindow->processAudio(samples, numsamples, bps, nch, srate);
}

int sf(int v)
{
	int res;
	res = v * (unsigned long)1103515245;
	res += (unsigned long)13293;
	res &= (unsigned long)0x7FFFFFFF;
	res ^= v;
	return res;
}

winampDSPModule *getModule(int which);

winampDSPHeader hdr = { DSP_HDRVER, "Example DSP Plug-In", getModule, sf };

winampDSPModule mod =
{
	"Example DSP Plug-In By CrownSoft",
	NULL,	// hwndParent
	NULL,	// hDllInstance
	config,
	init,
	modify_samples,
	quit
};

extern "C" __declspec(dllexport) winampDSPHeader *winampDSPGetHeader2()
{
	return &hdr;
}

winampDSPModule *getModule(int which)
{
	if (which == 0)
		return &mod;

	return NULL;
}
