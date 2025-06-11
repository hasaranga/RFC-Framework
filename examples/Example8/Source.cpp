
// Shows how to use a dialog box.

#include "rfc/rfc.h"
#include"resource.h"


class ExampleWindow : public KHotPluggedDialog
{
protected:
	KButton btn1, btn2;
	KTextBox txt;
	KComboBox combo;
	KIcon icon;

public:
	void OnHotPlug()
	{
		btn1.hotPlugInto(::GetDlgItem(compHWND, IDOK));
		btn1.onClick = [this](KButton* button) {
			::MessageBoxW(compHWND, txt.GetText(), L"yow...", 0);
		};

		btn2.hotPlugInto(::GetDlgItem(compHWND, IDCANCEL));
		btn2.onClick = [this](KButton* button) {
			this->onClose();
		};

		txt.hotPlugInto(::GetDlgItem(compHWND, IDC_EDIT1));

		combo.hotPlugInto(::GetDlgItem(compHWND, IDC_COMBO1));
		combo.addItem(L"Item1");
		combo.addItem(L"Item2");
		combo.addItem(L"Item3");
		combo.addItem(L"Item4");
		combo.selectItem(0);

		setText(L"RFC Subclassed Dialog & Controls");

		icon.loadFromResource(IDI_ICON1);
		setIcon(&icon);

		centerScreen();
	}

};


class ExampleApp : public KApplication
{
public:
	int main(wchar_t** argv, int argc)
	{
		ExampleWindow wnd;

		KGUIProc::hotPlugAndRunDialogBox(IDD_DIALOG1, 0, &wnd);

		return 0;
	}
};

START_RFC_APPLICATION(ExampleApp, KDPIAwareness::UNAWARE_MODE)