
// Shows how to use a dialog box.

#include "rfc/rfc.h"
#include"resource.h"


class ExampleWindow : public KHotPluggedDialog, public KButtonListener
{
protected:
	KButton btn1, btn2;
	KTextBox txt;
	KComboBox combo;
	KIcon icon;

public:
	void OnHotPlug()
	{
		btn1.HotPlugInto(GetDlgItem(compHWND, IDOK));
		btn1.SetListener(this);

		btn2.HotPlugInto(GetDlgItem(compHWND, IDCANCEL));
		btn2.SetListener(this);

		txt.HotPlugInto(GetDlgItem(compHWND, IDC_EDIT1));

		combo.HotPlugInto(GetDlgItem(compHWND, IDC_COMBO1));
		combo.AddItem(L"Item1");
		combo.AddItem(L"Item2");
		combo.AddItem(L"Item3");
		combo.AddItem(L"Item4");
		combo.SelectItem(0);

		this->SetText(L"RFC Subclassed Dialog & Controls");

		icon.LoadFromResource(IDI_ICON1);
		this->SetIcon(&icon);

		this->CenterScreen();
	}

	void OnButtonPress(KButton *button)
	{
		if (button == &btn1)
		{
			::MessageBoxW(compHWND, txt.GetText(), L"yow...", 0);
		}
		else if (button == &btn2)
		{
			this->OnClose();
		}
	}
};


class ExampleApp : public KApplication
{
public:
	int Main(KString **argv, int argc)
	{
		ExampleWindow wnd;

		KGUIProc::HotPlugAndRunDialogBox(IDD_DIALOG1, 0, &wnd);

		return 0;
	}
};

START_RFC_APPLICATION(ExampleApp, KDPIAwareness::UNAWARE_MODE)