
// Widgets Demo.

#include "rfc/rfc.h"

class TestGUI : public KFrame
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
	KFont fontWebdings;
	KFont fontSegoeUI;
	bool isStarted;

public:
	TestGUI()
	{
		isStarted = false;

		setText(L"Widgets Demo");
		setSize(600, 400);
		create();

		fontSegoeUI.load(L"Segoe UI", 14, false, false, false, true, this->getDPI()),
		fontWebdings.load(L"Webdings", 24, false, false, false, true, this->getDPI())

		btn1.setText(L"Button1");
		btn1.setPosition(10, 0);
		btn1.onClick = [this](KButton* sender) {
			::MessageBoxW(compHWND, textArea.getText(), textBox.getText(), 0);
		};

		btn2.setText(L"Button2");
		btn2.setPosition(10, 40);
		btn2.onClick = [this](KButton* sender) {
			KString fileName;
			if (KCommonDialogBox::showOpenFileDialog(this, L"title", KFILE_FILTER("Text Files", "txt"), &fileName))
				::MessageBoxW(compHWND, fileName, fileName, 0);
		};

		label.setPosition(120, 10);

		checkBox.setCheckedState(true);
		checkBox.setPosition(120, 40);

		radioButton.setCheckedState(true);
		radioButton.setPosition(230, 40);

		pushButton.setPosition(230, 0);

		menuButton.setText(L"Menu Button");
		menuButton.setGlyph(L"\x4A", &fontWebdings, RGB(0, 0, 255));
		menuButton.setSize(120, 30);
		menuButton.setPosition(350, 0);
		menuButton.setMenu(&fileMenu);

		glyphButton.setText(L"Glyph Button");
		glyphButton.setGlyph(L"\x34", &fontWebdings, RGB(34, 177, 76));
		glyphButton.setSize(120, 30);
		glyphButton.setPosition(350, 40);
		glyphButton.onClick = [this](KButton* sender) {
			if (isStarted)
			{
				glyphButton.setGlyph(L"\x34", &fontWebdings, RGB(34, 177, 76));
				isStarted = false;
			}
			else {
				glyphButton.setGlyph(L"\x3C", &fontWebdings, RGB(237, 28, 36));
				isStarted = true;
			}
		};

		groupBox.setPosition(10, 80);
		groupBox.setSize(100, 60);

		progressBar.setPosition(120, 90);
		progressBar.setValue(50);

		textBox.setText(L"text box");
		textBox.setPosition(120, 120);

		passwordBox.setText(L"pwd box");
		passwordBox.setPosition(230, 120);

		textArea.setText(L"text area");
		textArea.setPosition(10, 160);

		listBox.addItem(L"Item1");
		listBox.addItem(L"Item2");
		listBox.addItem(L"Item3");
		listBox.addItem(L"Item4");
		listBox.setPosition(230, 160);

		comboBox.addItem(L"Item1");
		comboBox.addItem(L"Item2");
		comboBox.addItem(L"Item3");
		comboBox.addItem(L"Item4");
		comboBox.selectItem(1);
		comboBox.setPosition(230, 90);

		trackBar.setValue(50);
		trackBar.setPosition(350, 90);

		gridView.setSize(220, 140);
		gridView.setPosition(350, 120);

		// we need to set custom font for standard controls to scale properly when dpi changes.
		btn1.setFont(&fontSegoeUI);
		btn2.setFont(&fontSegoeUI);
		label.setFont(&fontSegoeUI);
		checkBox.setFont(&fontSegoeUI);
		radioButton.setFont(&fontSegoeUI);
		pushButton.setFont(&fontSegoeUI);
		menuButton.setFont(&fontSegoeUI);
		glyphButton.setFont(&fontSegoeUI);
		groupBox.setFont(&fontSegoeUI);
		textBox.setFont(&fontSegoeUI);
		passwordBox.setFont(&fontSegoeUI);
		textArea.setFont(&fontSegoeUI);
		listBox.setFont(&fontSegoeUI);
		comboBox.setFont(&fontSegoeUI);
		gridView.setFont(&fontSegoeUI);
		trackBar.setFont(&fontSegoeUI);

		// set size,font and position of the components before adding them to window. 
		// so they will automatically rescale according to the current dpi.
		this->addComponent(&btn1);
		this->addComponent(&btn2);
		this->addComponent(&label);
		this->addComponent(&checkBox);
		this->addComponent(&radioButton);
		this->addComponent(&pushButton);
		this->addComponent(&groupBox);
		this->addComponent(&progressBar);
		this->addComponent(&textBox);
		this->addComponent(&passwordBox);
		this->addComponent(&textArea);
		this->addComponent(&listBox);
		this->addComponent(&comboBox);
		this->addComponent(&trackBar);
		this->addComponent(&menuButton);
		this->addComponent(&glyphButton);
		this->addComponent(&gridView);

		gridView.createColumn(L"User ID");
		gridView.createColumn(L"Name", 115);

		addRecords(L"1001", L"Don Box");
		addRecords(L"1002", L"Tony Williams");

		menuItem1.setText(L"New...");
		menuItem2.setText(L"Open...");

		fileMenu.addMenuItem(&menuItem1);
		fileMenu.addMenuItem(&menuItem2);
		fileMenu.addSeperator();
		fileMenu.addSubMenu(L"Sub Menu", &subMenu);

		subMenu.addMenuItem(&menuItem1);

		menuBar.addMenu(L"File", &fileMenu);

		menuBar.addToWindow(this);

		trackBarToolTip.setText(L"This is a Slider...");
		trackBarToolTip.attachToComponent(this, &trackBar);
	}

	void addRecords(KString userID, KString name)
	{
		KString* row[2];
		row[0] = &userID;
		row[1] = &name;

		gridView.insertRecord(row);
	}

	LRESULT onRClickWindow(WPARAM wParam, LPARAM lParam)
	{
		fileMenu.popUpMenu(*this);
		return 0;
	}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_RBUTTONUP, onRClickWindow)
	END_KMSG_HANDLER
};

class TestApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc)
	{
		TestGUI window1;

		window1.centerScreen();
		window1.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(TestApplication, KDPIAwareness::STANDARD_MODE);