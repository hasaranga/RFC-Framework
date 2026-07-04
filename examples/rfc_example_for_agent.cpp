

#include "rfc.h" // generate using combine_required_modules.bat

// KFrame class has close, minimize button. no maximize button. fixed size.
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
	KFontType ftWebdings;
	bool isStarted;

public:
	TestGUI() noexcept
	{
		isStarted = false;

		setText(L"Widgets Demo");
		setSize(600, 400);
		create();

		ftWebdings = KFontType(L"Webdings", 24, false, false, false, true);

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
		menuButton.setGlyph(L"\x4A", ftWebdings, RGB(0, 0, 255));
		menuButton.setSize(120, 30);
		menuButton.setPosition(350, 0);
		menuButton.setMenu(&fileMenu);

		glyphButton.setText(L"Glyph Button");
		glyphButton.setGlyph(L"\x34", ftWebdings, RGB(34, 177, 76));
		glyphButton.setSize(120, 30);
		glyphButton.setPosition(350, 40);
		glyphButton.onClick = [this](KButton* sender) {
			if (isStarted)
			{
				glyphButton.setGlyph(L"\x34", ftWebdings, RGB(34, 177, 76));
				isStarted = false;
			}
			else {
				glyphButton.setGlyph(L"\x3C", ftWebdings, RGB(237, 28, 36));
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

		addComponents(btn1, btn2, label, checkBox,
			radioButton, pushButton, groupBox, progressBar,
			textBox, passwordBox, textArea, listBox, comboBox,
			trackBar, menuButton, glyphButton, gridView);

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
		trackBarToolTip.attachToComponent(*this, trackBar);
	}

	void addRecords(KString userID, KString name) noexcept
	{
		KString* row[2];
		row[0] = &userID;
		row[1] = &name;

		gridView.insertRecord(row);
	}

	LRESULT onRClickWindow(WPARAM wParam, LPARAM lParam) noexcept
	{
		fileMenu.popUpMenu(*this);
		return 0;
	}

	// this macro allows quickly override the windowProc method.
	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_RBUTTONUP, onRClickWindow)
	END_KMSG_HANDLER
};

// To get HINSTANCE of the executable, use KApplication::hInstance.
// KApplication also allows to create single instance only applications.
// see core/KApplication.h for more information.
class TestApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc) noexcept
	{
		TestGUI window1;

		window1.centerScreen();
		window1.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

// the application is dpi aware. all widgets will be resized according to dpi.
START_RFC_APPLICATION(TestApplication, KDPIAwareness::STANDARD_MODE);