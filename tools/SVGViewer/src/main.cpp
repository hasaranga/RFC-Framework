

#include "rfc/rfc.h"
#include "SVGToHexConverter.h"

class MainWindow : public KDrawable<KFrame>
{
protected:
	KMenuBar menuBar;
	KMenu mFile;
	KMenuItem miOpen, miExit, miSaveAs;
	KSVGImage svgImage;
	KScopedClassPointer<KTransparentBitmap> transparentBitmap;
	KString filePath;
	const int IMAGE_GAP_96DPI = 20;
	int gapForCurrentDPI = IMAGE_GAP_96DPI;
	int imageWidth = 0;
	int imageHeight = 0;

	void onPaint(HDC hDCMem, RECT* rect, const int width, const int height) override
	{
		if (!transparentBitmap.isNull())
		{
			::FillRect(hDCMem, rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
			transparentBitmap->draw(hDCMem, gapForCurrentDPI, gapForCurrentDPI);
		}
		else
		{
			::FillRect(hDCMem, rect, (HBRUSH)::GetStockObject(DKGRAY_BRUSH));
		}
	}

	void generateImageForCurrentDPI()
	{
		int stride;
		unsigned char* data;

		svgImage.rasterize(getDPI());
		svgImage.getImageData(&imageWidth, &imageHeight, &data, &stride);
		transparentBitmap = new KTransparentBitmap(data, imageWidth, imageHeight, stride);
		svgImage.freeRasterData(); // since the data copied to transparentBitmap we can free raster data.

		gapForCurrentDPI = scaleToCurrentDPI(IMAGE_GAP_96DPI);
	}

public:
	MainWindow()
	{
		setText(L"SVG Viewer");
		setSize(370, 380);
		create();

		miOpen.setText(L"Open Image...");
		miExit.setText(L"Exit");
		miSaveAs.setText(L"Save As C/C++ Header...");

		miOpen.onPress = [this](KMenuItem* item) {
			if (KCommonDialogBox::showOpenFileDialog(this, L"Open SVG Image...", L"SVG Files (*.svg)\0*.svg\0", &this->filePath))
			{
				KScopedMallocPointer<char> ansiPath(KString::toAnsiString(filePath));
				if (svgImage.loadFromFile(ansiPath))
				{			
					generateImageForCurrentDPI();

					setClientAreaSize(imageWidth + gapForCurrentDPI * 2, imageHeight + gapForCurrentDPI * 2);
					setText(L"SVG Viewer - " + KFile::getFileNameFromPath(filePath));
					repaint();
				}
				else
				{
					::MessageBoxW(*this, L"File load failed!", L"Error", MB_ICONERROR);
				}
			}
		};

		miExit.onPress = [this](KMenuItem* item) {
			this->onClose();
		};

		miSaveAs.onPress = [this](KMenuItem* item) {
			if (filePath.length())
			{
				KString outputFilePath;
				if (KCommonDialogBox::showSaveFileDialog(this, L"Save As Header...", L"Header Files (*.h)\0*.h\0", &outputFilePath))
				{
					SVGToHexConverter hexConverter;
					KScopedMallocPointer<char> ansiSourcePath(KString::toAnsiString(filePath));
					KScopedMallocPointer<char> ansiDestPath(KString::toAnsiString(outputFilePath+".h"));

					std::filesystem::path path((char*)ansiDestPath);
					std::string arrayName = path.filename().string(); // extract file name without extension

					if (hexConverter.convertSVGToHeader((char*)ansiSourcePath, (char*)ansiDestPath, arrayName))
						::MessageBoxW(*this, L"Successfully converted!", L"Success", MB_ICONINFORMATION);
					else
						::MessageBoxW(*this, L"Conversion failed!", L"Error", MB_ICONERROR);
				}
			}
		};

		this->onDPIChange = [this](KWindow* window) {
			generateImageForCurrentDPI();
			repaint();
		};

		mFile.addMenuItem(&miOpen);
		mFile.addMenuItem(&miSaveAs);
		mFile.addSeperator();
		mFile.addMenuItem(&miExit);

		menuBar.addMenu(L"File", &mFile);
		menuBar.addToWindow(this);
	}

};

class MyApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc)
	{
		MainWindow window;
		window.centerScreen();
		window.setVisible(true);

		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);