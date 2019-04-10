

#include "../../amalgamated/rfc_amalgamated.h"

class TestGUI : public KFrame , public KButtonListener
{
protected:
	KButton btn1,btn2,btn3;
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
	KMenuItem menuItem1,menuItem2;
	KMenu fileMenu,subMenu;
	KMenuBar menuBar;
	KMenuButton menuButton;
	KGlyphButton glyphButton;
	KGridView gridView;
	KToolTip trackBarToolTip;
	KFont *fontWebdings24;
	bool isStarted;

public:
	TestGUI()
	{
		fontWebdings24 = new KFont(L"Webdings", 24,false,false,true);
		isStarted = false;

		this->Create();
		this->SetText(L"Demo");
		this->SetSize(600, 400);

		btn1.SetText(L"Button1");
		btn1.SetListener(this);
		btn1.SetPosition(10, 0);
		btn2.SetText(L"Button2");
		btn2.SetListener(this);
		btn2.SetPosition(10, 40);
		label.SetPosition(120,10);

		checkBox.SetCheckedState(true);
		checkBox.SetPosition(120, 40);
		
		radioButton.SetCheckedState(true);
		radioButton.SetPosition(230, 40);
		
		pushButton.SetPosition(230, 0);

		menuButton.SetText(L"Menu Button");
		menuButton.SetGlyph(L"\x4A", fontWebdings24, RGB(0, 0, 255));
		menuButton.SetSize(120, 30);
		menuButton.SetPosition(350, 0);
		menuButton.SetMenu(&fileMenu);

		glyphButton.SetText(L"Glyph Button");
		glyphButton.SetGlyph(L"\x34", fontWebdings24, RGB(34, 177, 76));
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
		gridView.SetSize(220, 140);
		gridView.SetPosition(350, 120);

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

	~TestGUI()
	{
		delete fontWebdings24;
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
		ON_KMSG(WM_RBUTTONUP,OnRClickWindow)
	END_KMSG_HANDLER(KFrame)

	void OnButtonPress(KButton *button)
	{
		if(button==&btn1)
		{
			::MessageBoxW(compHWND,textArea.GetText(),textBox.GetText(),0);
		}else if(button==&btn2)
		{
			KString fileName;
			if(KCommonDialogBox::ShowOpenFileDialog(this, L"title", KFILE_FILTER("Text Files", "txt"),&fileName))
			{
				::MessageBoxW(compHWND,fileName,fileName,0);
			}
		}
		else if (button == &glyphButton)
		{
			if (isStarted)
			{
				glyphButton.SetGlyph(L"\x34", fontWebdings24, RGB(34, 177, 76));
				isStarted = false;
			}
			else{
				glyphButton.SetGlyph(L"\x3C", fontWebdings24, RGB(237, 28, 36));
				isStarted = true;
			}
		}
	}

};

class MyApp : public KApplication
{
public:
	int Main(KString **argv,int argc)
	{
		TestGUI wnd;

		wnd.CenterScreen();
		wnd.SetVisible(true);
		
		::DoMessagePump();

		return 0;
	}
};

START_RFC_APPLICATION(MyApp);

