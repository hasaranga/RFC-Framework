
// Winamp DSP Plug-In Example

#include "../../amalgamated/rfc_amalgamated.h"
#include"resource.h"
#include "dsp.h" 


#define MY_FORMAT_ID KFORMAT_ID('dsp1')

class PluginWindow : public KHotPluggedDialog, public KButtonListener
{
protected:
	KCheckBox chkEnableProcessing;
	KString settingsFile;
	volatile bool processAudioFlag;

public:
	void OnHotPlug()
	{
		KHotPluggedDialog::OnHotPlug();

		processAudioFlag = false;

		chkEnableProcessing.HotPlugInto(::GetDlgItem(compHWND, IDC_CHECK1));
		chkEnableProcessing.SetCheckedState(false);
		chkEnableProcessing.SetListener(this);		

		KString appDataDir = KDirectory::GetApplicationDataDir() + L"\\TestWinampDSP1";
		KDirectory::CreateDir(appDataDir);

		settingsFile = appDataDir + L"\\settings.conf";

		// load settings
		KSettingsReader settingsReader;
		if (settingsReader.OpenFile(settingsFile, MY_FORMAT_ID))
		{
			processAudioFlag = settingsReader.ReadBool();
			chkEnableProcessing.SetCheckedState(processAudioFlag);

			int x = settingsReader.ReadInt();
			int y = settingsReader.ReadInt();
			this->SetPosition(x, y);

			bool visible = settingsReader.ReadBool();
			this->SetVisible(visible);
		}

	}

	void OnButtonPress(KButton *button)
	{
		if (button == &chkEnableProcessing)
		{
			processAudioFlag = chkEnableProcessing.IsChecked();
		}
	}

	void SaveSettings()
	{
		KSettingsWriter settingsWriter;

		// save settings
		if (settingsWriter.OpenFile(settingsFile, MY_FORMAT_ID))
		{
			settingsWriter.WriteBool(processAudioFlag);
			settingsWriter.WriteInt(this->GetX());
			settingsWriter.WriteInt(this->GetY());
			settingsWriter.WriteBool(this->IsVisible());
		}
	}

	int ProcessAudio(short int *samples, int numsamples, int bps, int nch, int srate)
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

	void OnClose()
	{
		this->SetVisible(false);
	}
};

PluginWindow *pluginWindow;

int init(struct winampDSPModule *this_mod)
{
	::InitRFC(this_mod->hDllInstance);

	pluginWindow = new PluginWindow();
	::HotPlugAndCreateDialogBox(IDD_DIALOG1, this_mod->hwndParent, pluginWindow);

	return 0;
}

void config(struct winampDSPModule *this_mod)
{
	pluginWindow->SetVisible(true);
	pluginWindow->BringToFront();
}

void quit(struct winampDSPModule *this_mod)
{
	pluginWindow->SaveSettings();
	pluginWindow->Destroy();

	delete pluginWindow;
	::DeInitRFC();
}

int modify_samples(struct winampDSPModule *this_mod, short int *samples, int numsamples, int bps, int nch, int srate)
{
	return pluginWindow->ProcessAudio(samples, numsamples, bps, nch, srate);
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
