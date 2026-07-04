
#include "rfc.h"

class MyApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc)
	{
		KString totpKey = KTOTPAuth::generateTOTPKey();
		KString qrCodeText = KTOTPAuth::generateQRText(totpKey, L"Example App", L"user@abc.com");

		KWithOnCloseEvent<KFrame> window;
		window.setText(L"TOTP Authentication");
		window.setSize(370, 160);
		window.create();
		window.centerScreen();

		KQRCodeBox qrBox;
		qrBox.setPosition(10, 10);
		qrBox.setText(qrCodeText);
		
		KFont fontTOTPCode(L"Segoe UI", 30, true);
		KFont fontNormal(L"Segoe UI", 14);

		KLabel lblAuthCode;
		lblAuthCode.setFont(&fontTOTPCode);
		lblAuthCode.enableAutoResize(true);
		lblAuthCode.placeToRightOf(qrBox, 50);
		lblAuthCode.setText(KTOTPAuth::getTOTPCodeForCurrentTime(totpKey));
		
		KNumericField txtCode;
		txtCode.setFont(&fontNormal);
		txtCode.placeBelow(lblAuthCode, 10);

		KButton btnValidate;
		btnValidate.setFont(&fontNormal);
		btnValidate.setSize(55, txtCode.getHeight());
		btnValidate.placeToRightOf(txtCode, 10);
		btnValidate.setText(L"Validate");

		KLabel lblStatus;
		lblStatus.setFont(&fontNormal);
		lblStatus.enableAutoResize(true);
		lblStatus.setText(L"scan QR code using auth app.");
		lblStatus.placeBelow(txtCode, 20);

		KLabel lblTime;
		lblTime.setFont(&fontNormal);
		lblTime.enableAutoResize(true);
		lblTime.setText(L"   ");
		lblTime.alignCenterVerticallyWith(lblAuthCode);
		lblTime.alignCenterHorizontallyWith(btnValidate);

		window.addComponents(qrBox, lblAuthCode, lblTime, 
			txtCode, btnValidate, lblStatus);

		btnValidate.onClick = [&totpKey, &lblStatus, &txtCode](KButton* sender) {
			KString userText = txtCode.getText();
			if (userText.length() != 6)
			{
				lblStatus.setText(L"invalid value entered!");
				return;
			}

			int result = KTOTPAuth::validateTOTPCode(totpKey, userText);
			if(result == 0)
				lblStatus.setText(L"invalid code!");
			else if (result == 1)
				lblStatus.setText(L"valid code (current)");
			else if (result == 2)
				lblStatus.setText(L"valid code (previous)");
			else if (result == 3)
				lblStatus.setText(L"valid code (next)");
		};

		KProperty<int> remainingSeconds(0);
		remainingSeconds.onChange = [&lblTime](int newValue) {
			lblTime.setText(KString(newValue) + L"s");
		};

		KProperty<bool> shouldGenerateCode(false);
		shouldGenerateCode.onChange = [&lblAuthCode, &totpKey](bool newValue) {
			if(newValue)
				lblAuthCode.setText(KTOTPAuth::getTOTPCodeForCurrentTime(totpKey));
		};
		
		int lastRemaining = -1;
		KTimer totpCodeTimer;
		totpCodeTimer.start(1000, window, [&remainingSeconds, &lastRemaining, &shouldGenerateCode](KTimer* sender) {
			// the timer just updates the state, and the UI automatically updates when the state changes.
			remainingSeconds = KTOTPAuth::getRemainingSeconds();
			shouldGenerateCode = (lastRemaining != -1 && remainingSeconds > lastRemaining);
			lastRemaining = remainingSeconds;		
		});

		window.onCloseEvent = [&totpCodeTimer]() {
			totpCodeTimer.stop();
		};

		window.setVisible(true);
		KApplication::messageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);