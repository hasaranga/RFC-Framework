
// Widgets Demo.

#include "rfc/rfc.h"

class TestGUI : public KFrame, public KButtonListener
{
protected:
	KButton btn1, btn2, btn3;
	KLabel label;
	KCheckBox checkBox;
	KRadioButton radioButton;
	KPushButton pushButton;
	KGroupBox groupBox;
	KProgressBar progressBar;
	KTextBox textBox;
	KPasswordBox passwordBox;
	KTextArea textArea;
	KListBox listBox;
	KComboBox comboBox;
	KTrackBar trackBar;
	KMenuItem menuItem1, menuItem2;
	KMenu fileMenu, subMenu;
	KMenuBar menuBar;
	KMenuButton menuButton;
	KGlyphButton glyphButton;
	KGridView gridView;
	KToolTip trackBarToolTip;
	KScopedClassPointer<KFont> fontWebdings;
	KScopedClassPointer<KFont> fontSegoeUI;
	bool isStarted;

public:
	TestGUI()
	{
		isStarted = false;

		this->SetText(L"Widgets Demo");
		this->SetSize(600, 400);
		this->Create();

		// create fonts after creating the window. So GetDPI will return the current dpi of the screen.
		fontSegoeUI = new KFont(L"Segoe UI", 14, false, false, false, true, this->GetDPI());
		fontWebdings = new KFont(L"Webdings", 24, false, false, false, true, this->GetDPI());

		btn1.SetText(L"Button1");
		btn1.SetListener(this);
		btn1.SetPosition(10, 0);
		btn2.SetText(L"Button2");
		btn2.SetListener(this);
		btn2.SetPosition(10, 40);
		label.SetPosition(120, 10);

		checkBox.SetCheckedState(true);
		checkBox.SetPosition(120, 40);

		radioButton.SetCheckedState(true);
		radioButton.SetPosition(230, 40);

		pushButton.SetPosition(230, 0);

		menuButton.SetText(L"Menu Button");
		menuButton.SetGlyph(L"\x4A", fontWebdings, RGB(0, 0, 255));
		menuButton.SetSize(120, 30);
		menuButton.SetPosition(350, 0);
		menuButton.SetMenu(&fileMenu);

		glyphButton.SetText(L"Glyph Button");
		glyphButton.SetGlyph(L"\x34", fontWebdings, RGB(34, 177, 76));
		glyphButton.SetSize(120, 30);
		glyphButton.SetPosition(350, 40);
		glyphButton.SetListener(this);

		groupBox.SetPosition(10, 80);
		groupBox.SetSize(100, 60);

		progressBar.SetPosition(120, 90);
		progressBar.SetValue(50);

		textBox.SetText(L"text box");
		textBox.SetPosition(120, 120);

		passwordBox.SetText(L"pwd box");
		passwordBox.SetPosition(230, 120);

		textArea.SetText(L"text area");
		textArea.SetPosition(10, 160);

		listBox.AddItem(L"Item1");
		listBox.AddItem(L"Item2");
		listBox.AddItem(L"Item3");
		listBox.AddItem(L"Item4");
		listBox.SetPosition(230, 160);

		comboBox.AddItem(L"Item1");
		comboBox.AddItem(L"Item2");
		comboBox.AddItem(L"Item3");
		comboBox.AddItem(L"Item4");
		comboBox.SelectItem(1);
		comboBox.SetPosition(230, 90);

		trackBar.SetValue(50);
		trackBar.SetPosition(350, 90);

		gridView.SetSize(220, 140);
		gridView.SetPosition(350, 120);

		// we need to set custom font for standard controls to scale properly when dpi changes.
		btn1.SetFont(fontSegoeUI);
		btn2.SetFont(fontSegoeUI);
		label.SetFont(fontSegoeUI);
		checkBox.SetFont(fontSegoeUI);
		radioButton.SetFont(fontSegoeUI);
		pushButton.SetFont(fontSegoeUI);
		menuButton.SetFont(fontSegoeUI);
		glyphButton.SetFont(fontSegoeUI);
		groupBox.SetFont(fontSegoeUI);
		textBox.SetFont(fontSegoeUI);
		passwordBox.SetFont(fontSegoeUI);
		textArea.SetFont(fontSegoeUI);
		listBox.SetFont(fontSegoeUI);
		comboBox.SetFont(fontSegoeUI);
		gridView.SetFont(fontSegoeUI);
		trackBar.SetFont(fontSegoeUI);

		// set size,font and position of the components before adding them to window. 
		// so they will automatically rescale according to the current dpi.
		this->AddComponent(&btn1);
		this->AddComponent(&btn2);
		this->AddComponent(&label);
		this->AddComponent(&checkBox);
		this->AddComponent(&radioButton);
		this->AddComponent(&pushButton);
		this->AddComponent(&groupBox);
		this->AddComponent(&progressBar);
		this->AddComponent(&textBox);
		this->AddComponent(&passwordBox);
		this->AddComponent(&textArea);
		this->AddComponent(&listBox);
		this->AddComponent(&comboBox);
		this->AddComponent(&trackBar);
		this->AddComponent(&menuButton);
		this->AddComponent(&glyphButton);
		this->AddComponent(&gridView);

		gridView.CreateColumn(L"User ID");
		gridView.CreateColumn(L"Name", 115);

		AddRecords(L"1001", L"Don Box");
		AddRecords(L"1002", L"Tony Williams");

		menuItem1.SetText(L"New...");
		menuItem2.SetText(L"Open...");

		fileMenu.AddMenuItem(&menuItem1);
		fileMenu.AddMenuItem(&menuItem2);
		fileMenu.AddSeperator();
		fileMenu.AddSubMenu(L"Sub Menu", &subMenu);

		subMenu.AddMenuItem(&menuItem1);

		menuBar.AddMenu(L"File", &fileMenu);

		menuBar.AddToWindow(this);

		trackBarToolTip.SetText(L"This is a Slider...");
		trackBarToolTip.AttachToComponent(this, &trackBar);
	}

	void AddRecords(KString userID, KString name)
	{
		KString* row[2];
		row[0] = &userID;
		row[1] = &name;

		gridView.InsertRecord(row);
	}

	LRESULT OnRClickWindow(WPARAM wParam, LPARAM lParam)
	{
		fileMenu.PopUpMenu(this);
		return 0;
	}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_RBUTTONUP, OnRClickWindow)
	END_KMSG_HANDLER(KFrame)

	void OnButtonPress(KButton* button)
	{
		if (button == &btn1)
		{
			::MessageBoxW(compHWND, textArea.GetText(), textBox.GetText(), 0);
		}
		else if (button == &btn2)
		{
			KString fileName;
			if (KCommonDialogBox::ShowOpenFileDialog(this, L"title", KFILE_FILTER("Text Files", "txt"), &fileName))
			{
				::MessageBoxW(compHWND, fileName, fileName, 0);
			}
		}
		else if (button == &glyphButton)
		{
			if (isStarted)
			{
				glyphButton.SetGlyph(L"\x34", fontWebdings, RGB(34, 177, 76));
				isStarted = false;
			}
			else {
				glyphButton.SetGlyph(L"\x3C", fontWebdings, RGB(237, 28, 36));
				isStarted = true;
			}
		}
	}

};

class TestApplication : public KApplication
{
public:
	int Main(KString** argv, int argc)
	{
		TestGUI window1;

		window1.CenterScreen();
		window1.SetVisible(true);

		KApplication::MessageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(TestApplication, KDPIAwareness::STANDARD_MODE);