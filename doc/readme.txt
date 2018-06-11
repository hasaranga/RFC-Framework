
RFC Framework v0.2.6
Copyright (C) 2013-2018 CrownSoft


RFC Framework is a C++ framework which helps you to create Windows GUI applications with clean and minimum code.
By using this framework, you can write applications using object oriented fashion!

copy rfc_amalgamated.h and rfc_amalgamated.cpp files into your project and start coding!
Checkout examples dir for usage of this framework.

The minimum supported OS is Windows XP.

By using HotPlugging technique, you can use Dialog designer in visual studio to create RFC applications!

Tips for HotPlugging/Subclassing:

			(*) use KHotPluggedDialog to HotPlug dialog box.
			(*) use OnHotPlug(act as a constructor and you will not recieve WM_INITDIALOG msg!) method to 
			    initialize and HotPlug child components.
			(*) no need to call DoMessageLoop function, because HotPlugAndRunDialogBox runs message loop internally.
			    
Send your bug reports/suggestions to support@crownsoft.net

Logo designed by Fernando Augusto Birck (http://www.fergonez.net)

Visit http://www.crownsoft.net for more information.