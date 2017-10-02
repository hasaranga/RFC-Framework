

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

public:
	TestGUI()
	{
		this->CreateComponent();
		this->SetText(L"Demo");
		this->SetSize(600, 400);

		btn1.SetText(L"Button1");
		btn1.SetListener(this);
		btn2.SetText(L"Button2");
		btn2.SetListener(this);
		btn2.SetPosition(0,40);

		label.SetPosition(120,10);

		checkBox.SetCheckedState(true);
		checkBox.SetPosition(120,40);
		
		radioButton.SetCheckedState(true);
		radioButton.SetPosition(230,40);
		
		pushButton.SetPosition(230,0);
		
		groupBox.SetPosition(10,80);
		groupBox.SetSize(100,60);
		
		progressBar.SetPosition(120,90);
		progressBar.SetValue(50);
		
		textBox.SetText(L"text box");
		textBox.SetPosition(120,120);

		passwordBox.SetText(L"pwd box");
		passwordBox.SetPosition(230,120);

		textArea.SetText(L"text area");
		textArea.SetPosition(10,160);

		listBox.AddItem(L"Item1");
		listBox.AddItem(L"Item2");
		listBox.AddItem(L"Item3");
		listBox.AddItem(L"Item4");
		listBox.SetPosition(230,160);

		comboBox.AddItem(L"Item1");
		comboBox.AddItem(L"Item2");
		comboBox.AddItem(L"Item3");
		comboBox.AddItem(L"Item4");
		comboBox.SelectItem(1);
		comboBox.SetPosition(230,90);

		trackBar.SetValue(50);
		trackBar.SetPosition(330,90);	

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

		menuItem1.SetText(L"New...");
		menuItem2.SetText(L"Open...");
	
		fileMenu.AddMenuItem(&menuItem1);
		fileMenu.AddMenuItem(&menuItem2);
		fileMenu.AddSeperator();
		fileMenu.AddSubMenu(L"Sub Menu",&subMenu);

		subMenu.AddMenuItem(&menuItem1);
		
		menuBar.AddMenu(L"File",&fileMenu);

		menuBar.AddToWindow(this);
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
			if(KCommonDialogBox::ShowOpenFileDialog(this,L"title",L"Text Files (*.txt)\0*.txt\0",&fileName))
			{
				::MessageBoxW(compHWND,fileName,fileName,0);
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

