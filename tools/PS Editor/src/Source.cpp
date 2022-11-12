
// Property Storage Editor v1.0
// (c) 2022 CrownSoft

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include "lunasvg/lunasvg.h"
#include "resource.h"

#include "../../../rfc/io/KPropertyStorage.h"

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib, "Shlwapi.lib")

HWND hwndTree;
HFONT hFontTree;

HWND hwndObjectDlg;
HWND hwndObjectDlg_Edit_ObjID;
HWND hwndObjectDlg_Edit_ObjName;
HWND hwndObjectDlg_Btn_Save;

HWND hwndPropertyDlg;
HWND hwndPropertyDlg_Edit_PropName;
HWND hwndPropertyDlg_Cmb_PropType;
HWND hwndPropertyDlg_Btn_SVGPreview;
HWND hwndPropertyDlg_Edit_PropValue;
HWND hwndPropertyDlg_Btn_Save;
HWND hwndPropertyDlg_Btn_Browse;
HWND hwndPropertyDlg_Lbl_FileName;

HWND hwndPreviewDlg_Cmb_Scale;

HINSTANCE hInstance;

std::unique_ptr<lunasvg::Document> svgDocument;
Gdiplus::Bitmap* previewBitmap = nullptr;
lunasvg::Bitmap lunaBitmap;

#define W_KEY_ID 1
#define S_KEY_ID 2

class TreeViewItemParam
{
public:
    int type;
    void* data;
};

class KPSPropertyView2 : public KPSPropertyView
{
public:
    HTREEITEM hTree;
    TreeViewItemParam* param;

    KPSPropertyView2()
    {
        hTree = 0;
        param = NULL;
    }

    virtual ~KPSPropertyView2()
    {
        if (param)
            delete param;
    }
};

class KPSObjectView2 : public KPSObjectView
{
public:
    HTREEITEM hTree;
    TreeViewItemParam* param;

    KPSObjectView2(int initialPropertyCount) : KPSObjectView(initialPropertyCount)
    {
        hTree = 0;
        param = NULL;
    }

    virtual ~KPSObjectView2()
    {
        if (param)
            delete param;
    }
};

KPointerList<KPSObjectView2*> objectList;

void setClientAreaSize(HWND hwndDlg, int width, int height)
{
    RECT wndRect;
    ::GetClientRect(hwndDlg, &wndRect);

    wndRect.right = wndRect.left + width;
    wndRect.bottom = wndRect.top + height;

    ::AdjustWindowRect(&wndRect, (DWORD)::GetWindowLongPtrW(hwndDlg, GWL_STYLE), FALSE);
    ::SetWindowPos(hwndDlg, 0, 0, 0, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, 
        SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void generatePreview(HWND hwndDlg, int scale)
{
    const int TOP_SECTION_HEIGHT = 45;

    if (svgDocument) // valid svg code
    {
        const int newWidth = (int)((scale / 100.0f) * svgDocument->width());
        const int newHeight = (int)((scale / 100.0f) * svgDocument->height());
        lunaBitmap = svgDocument->renderToBitmap(newWidth, newHeight);

        Gdiplus::Bitmap* scaledImage = new Gdiplus::Bitmap(lunaBitmap.width(), lunaBitmap.height(),
            (INT)lunaBitmap.stride(), PixelFormat32bppPARGB, (BYTE*)lunaBitmap.data());

        int newClientAreaWidth = lunaBitmap.width() + 20 + 20;
        int newClientAreaHeight = lunaBitmap.height() + 20 + 20 + TOP_SECTION_HEIGHT;

        const int minimumClientAreaWidth = 400;
        const int minimumClientAreaHeight = 200;

        if (newClientAreaWidth < minimumClientAreaWidth)
            newClientAreaWidth = minimumClientAreaWidth;

        if (newClientAreaHeight < minimumClientAreaHeight)
            newClientAreaHeight = minimumClientAreaHeight;

        if (previewBitmap)
            delete previewBitmap;

        previewBitmap = new Gdiplus::Bitmap(newClientAreaWidth, newClientAreaHeight);
        Gdiplus::Graphics previewGraphics(previewBitmap);

        COLORREF backColor = ::GetSysColor(COLOR_3DFACE);
        Gdiplus::SolidBrush topBrush(Gdiplus::Color(GetRValue(backColor), GetGValue(backColor), GetBValue(backColor)));
        previewGraphics.FillRectangle(&topBrush, 0, 0, newClientAreaWidth, TOP_SECTION_HEIGHT);

        Gdiplus::HatchBrush bottomBrush(Gdiplus::HatchStyleSmallGrid, Gdiplus::Color(255,240,240,240), Gdiplus::Color::White);
        previewGraphics.FillRectangle(&bottomBrush, 0, TOP_SECTION_HEIGHT, newClientAreaWidth, newClientAreaHeight - TOP_SECTION_HEIGHT);

        previewGraphics.DrawImage(scaledImage, (newClientAreaWidth - scaledImage->GetWidth()) / 2,
            TOP_SECTION_HEIGHT + (((newClientAreaHeight - TOP_SECTION_HEIGHT) - scaledImage->GetHeight()) / 2),
            0,0, scaledImage->GetWidth(), scaledImage->GetHeight(), Gdiplus::Unit::UnitPixel);
    
        delete scaledImage;
        setClientAreaSize(hwndDlg, newClientAreaWidth, newClientAreaHeight);
    }
    else // invalid svg code
    {
        if (previewBitmap)
            delete previewBitmap;

        previewBitmap = new Gdiplus::Bitmap(400, 200);
        Gdiplus::Graphics previewGraphics(previewBitmap);

        COLORREF backColor = ::GetSysColor(COLOR_3DFACE);
        Gdiplus::SolidBrush topBrush(Gdiplus::Color(GetRValue(backColor), GetGValue(backColor), GetBValue(backColor)));
        previewGraphics.FillRectangle(&topBrush, 0, 0, 400, TOP_SECTION_HEIGHT);

        Gdiplus::HatchBrush bottomBrush(Gdiplus::HatchStyleSmallGrid, Gdiplus::Color(255, 240, 240, 240), Gdiplus::Color::White);
        previewGraphics.FillRectangle(&bottomBrush, 0, TOP_SECTION_HEIGHT, 400, 200 - TOP_SECTION_HEIGHT);

        // Create a string.
        WCHAR text[] = L"Image Error";

        // Initialize arguments.
        Gdiplus::Font myFont(L"System", 12);
        Gdiplus::PointF origin(160, TOP_SECTION_HEIGHT+60);
        Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 255, 0, 0));

        // Draw string.
        previewGraphics.DrawString(
            text,
            11,
            &myFont,
            origin,
            &blackBrush);

        setClientAreaSize(hwndDlg, 400, 200);
    }

    ::InvalidateRect(hwndDlg, NULL, FALSE);
    ::UpdateWindow(hwndDlg);
}

INT_PTR CALLBACK PreviewDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        hwndPreviewDlg_Cmb_Scale = ::GetDlgItem(hwndDlg, IDC_COMBO1);

        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"100%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"125%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"150%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"175%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"200%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"225%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"250%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"275%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_ADDSTRING, 0, (LPARAM)L"300%");
        ::SendMessageW(hwndPreviewDlg_Cmb_Scale, CB_SETCURSEL, 0, 0);

        char* svgText = (char*)lParam;
        svgDocument = lunasvg::Document::loadFromData(svgText);

        generatePreview(hwndDlg, 100);

        return FALSE;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hwndDlg, &ps);

        if (previewBitmap)
        {
            Gdiplus::Graphics graphics(hdc);
            graphics.DrawImage(previewBitmap, 0, 0, previewBitmap->GetWidth(), previewBitmap->GetHeight());
        }

        ::EndPaint(hwndDlg, &ps);
        return TRUE;
    }

    case WM_ERASEBKGND:
        return TRUE;

    case WM_COMMAND:
    {
        if (lParam)
        {
            if ((LOWORD(wParam) == IDC_COMBO1) && (HIWORD(wParam) == CBN_SELCHANGE))
            {
                if (svgDocument)
                {
                    int itemIndex = (int)SendMessageW(hwndPreviewDlg_Cmb_Scale, (UINT)CB_GETCURSEL,
                        (WPARAM)0, (LPARAM)0);

                    int scaleList[] = { 100, 125,150,175,200,225,250,275,300 };
                    generatePreview(hwndDlg, scaleList[itemIndex]);
                }
            }
        }
        return FALSE;
    }

    case WM_CLOSE:
        ::EndDialog(hwndDlg, 0);
        svgDocument.reset();

        if (previewBitmap)
            delete previewBitmap;
        previewBitmap = nullptr;

        return TRUE;

    default:
        return FALSE;
    }
}

INT_PTR CALLBACK PropertyDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        hwndPropertyDlg_Edit_PropName = ::GetDlgItem(hwndDlg, IDC_EDIT1);
        hwndPropertyDlg_Cmb_PropType = ::GetDlgItem(hwndDlg, IDC_COMBO1);
        hwndPropertyDlg_Btn_SVGPreview = ::GetDlgItem(hwndDlg, IDC_BUTTON1);
        hwndPropertyDlg_Edit_PropValue = ::GetDlgItem(hwndDlg, IDC_EDIT2);
        hwndPropertyDlg_Btn_Save = ::GetDlgItem(hwndDlg, IDC_BUTTON2);
        hwndPropertyDlg_Btn_Browse = ::GetDlgItem(hwndDlg, IDC_BUTTON3);
        hwndPropertyDlg_Lbl_FileName = ::GetDlgItem(hwndDlg, IDC_FILE_NAME);

        ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_ADDSTRING, 0, (LPARAM)L"String");
        ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_ADDSTRING, 0, (LPARAM)L"Integer");
        ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_ADDSTRING, 0, (LPARAM)L"DWORD");
        ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_ADDSTRING, 0, (LPARAM)L"Float");
        ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_ADDSTRING, 0, (LPARAM)L"Int Array");
        ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_ADDSTRING, 0, (LPARAM)L"GUID");
        ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_ADDSTRING, 0, (LPARAM)L"File");

        return FALSE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON2) // update button
        {
            const int propertyNameBufferSize = ::GetWindowTextLengthW(hwndPropertyDlg_Edit_PropName);
            if (propertyNameBufferSize == 0)
            {
                ::MessageBoxW(hwndDlg, L"Invalid Property Name!", L"Error", MB_ICONERROR);
                return TRUE;
            }

            wchar_t* propertyNameStr = (wchar_t*)::malloc(sizeof(wchar_t) * (propertyNameBufferSize + 1));
            ::GetWindowTextW(hwndPropertyDlg_Edit_PropName, propertyNameStr, propertyNameBufferSize + 1);

            const int propertyNameStrLength = (int)::wcslen(propertyNameStr);
            if (propertyNameStrLength < 1)
            {
                ::free(propertyNameStr);
                ::MessageBoxW(hwndDlg, L"Invalid Property Name!", L"Error", MB_ICONERROR);
                return TRUE;
            }     

            wchar_t* propertyValueStr = NULL;
            int propertyValueStrLength = 0;

            const int cmbSelectedIndex = (int)::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_GETCURSEL, 0, 0);

            if (cmbSelectedIndex != KPSPropertyTypes::FILE) // not a file
            {
                const int propertyValueBufferSize = ::GetWindowTextLengthW(hwndPropertyDlg_Edit_PropValue);
                if (propertyValueBufferSize == 0)
                {
                    ::free(propertyNameStr);
                    ::MessageBoxW(hwndDlg, L"Invalid Property Value!", L"Error", MB_ICONERROR);
                    return TRUE;
                }

                propertyValueStr = (wchar_t*)::malloc(sizeof(wchar_t) * (propertyValueBufferSize + 1));
                ::GetWindowTextW(hwndPropertyDlg_Edit_PropValue, propertyValueStr, propertyValueBufferSize + 1);

                propertyValueStrLength = (int)::wcslen(propertyValueStr);
                if (propertyValueStrLength < 1)
                {
                    ::free(propertyNameStr);
                    ::free(propertyValueStr);
                    ::MessageBoxW(hwndDlg, L"Invalid Property Value!", L"Error", MB_ICONERROR);
                    return TRUE;
                }

                ::SetWindowTextW(hwndPropertyDlg_Lbl_FileName, L""); // clear file name label
            }
            else // file
            {
                ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, L""); // clear property value
            }

            HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

            TVITEMW item = { 0 };
            item.hItem = hSelectedItem;
            item.mask = TVIF_PARAM;

            TreeView_GetItem(hwndTree, &item);

            TreeViewItemParam* param = (TreeViewItemParam*)item.lParam;
            KPSPropertyView2* psProperty = (KPSPropertyView2*)param->data;
            
            psProperty->type = cmbSelectedIndex;       

            if (psProperty->name)
            {
                ::free(psProperty->name);
                psProperty->name = NULL;
            }

            if (psProperty->strValue)
            {
                ::free(psProperty->strValue);
                psProperty->strValue = NULL;
            }

            if (psProperty->intArray)
            {
                ::free(psProperty->intArray);
                psProperty->intArray = NULL;
            }

            psProperty->name = propertyNameStr;
            psProperty->nameLength = propertyNameStrLength;

            if (cmbSelectedIndex == KPSPropertyTypes::STRING)  // string
            {
                psProperty->strValue = propertyValueStr;
                psProperty->strValueLength = propertyValueStrLength;
            }
            else if (cmbSelectedIndex == KPSPropertyTypes::INTEGER) // int
            {
                psProperty->intValue = ::_wtoi(propertyValueStr);
                ::free(propertyValueStr);

                wchar_t buffer[16];
                ::_itow_s(psProperty->intValue, buffer, 16, 10);
                ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, buffer);
            }
            else if (cmbSelectedIndex == KPSPropertyTypes::DWORD) // DWORD
            {
                psProperty->dwordValue = 0;
                ::swscanf_s(propertyValueStr, L"0x%08x", &psProperty->dwordValue);
                
                ::free(propertyValueStr);

                wchar_t buffer[16];
                ::swprintf(buffer, 16, L"0x%08x", psProperty->dwordValue);
                ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, buffer);
            }
            else if (cmbSelectedIndex == KPSPropertyTypes::FLOAT) // float
            {
                psProperty->floatValue = 0;
                ::swscanf_s(propertyValueStr, L"%f", &psProperty->floatValue);

                ::free(propertyValueStr);

                wchar_t buffer[64];
                ::swprintf(buffer, 64, L"%f", psProperty->floatValue);
                ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, buffer);
            }
            else if (cmbSelectedIndex == KPSPropertyTypes::INT_ARRAY) // int array
            {
                psProperty->GenerateIntArrayByString(propertyValueStr);
                ::free(propertyValueStr);

                ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, psProperty->intArrayStr);
            }
            else if(cmbSelectedIndex == KPSPropertyTypes::GUID) // GUID
            {
                psProperty->GenerateGUIDValueByString(propertyValueStr);
                ::free(propertyValueStr);

                ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, psProperty->guidValueStr);
            }
            // file is updated when pressing browse button!

            TV_ITEMW updateItem = { 0 };
            updateItem.hItem = hSelectedItem;
            updateItem.mask = TVIF_TEXT;
            updateItem.pszText = psProperty->name;
            updateItem.cchTextMax = psProperty->nameLength;

            ::SendMessageW(hwndTree, TVM_SETITEMW, 0, (LPARAM)&updateItem);

            return TRUE;
        }
        else if (LOWORD(wParam) == IDC_BUTTON1) // preview button
        {
            const int cmbSelectedIndex = (int)::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_GETCURSEL, 0, 0);

            if (cmbSelectedIndex != KPSPropertyTypes::STRING) // not a string type
                return TRUE;

            const int propertyValueBufferSize = ::GetWindowTextLengthA(hwndPropertyDlg_Edit_PropValue);
            if (propertyValueBufferSize == 0)
                return TRUE;

            char* propertyValueStr = (char*)::malloc(propertyValueBufferSize + 1);
            ::GetWindowTextA(hwndPropertyDlg_Edit_PropValue, propertyValueStr, propertyValueBufferSize + 1);

            const int propertyValueStrLength = (const int)::strlen(propertyValueStr);
            if (propertyValueStrLength < 1)
            {
                ::free(propertyValueStr);
                return TRUE;
            }

            ::DialogBoxParamW(GetModuleHandle(NULL), MAKEINTRESOURCEW(IDD_DIALOG4), ::GetParent(hwndDlg), PreviewDlgProc, (LPARAM)propertyValueStr);

            ::free(propertyValueStr);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDC_BUTTON3) // browse button
        {
            const int cmbSelectedIndex = (int)::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_GETCURSEL, 0, 0);

            if (cmbSelectedIndex != KPSPropertyTypes::FILE) // not a file type
                return TRUE;

            OPENFILENAMEW ofn;
            ::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

            // assumes MAX_PATH * 2 is enough!
            wchar_t* fileName = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
            fileName[0] = 0;

            ofn.lStructSize = sizeof(OPENFILENAMEW);
            ofn.hwndOwner = hwndDlg;
            ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH * 2;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            ofn.lpstrTitle = L"Add File...";

            if (::GetOpenFileNameW(&ofn))
            {
                HANDLE fileHandle = ::CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                if (fileHandle != INVALID_HANDLE_VALUE)
                {
                    DWORD fileSize = ::GetFileSize(fileHandle, NULL);
                    fileSize = (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;

                    if (fileSize)
                    {
                        void* buffer = (void*)::malloc(fileSize);
                        DWORD bytesRead;
                        ::ReadFile(fileHandle, buffer, fileSize, &bytesRead, NULL);

                        HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

                        TVITEMW item = { 0 };
                        item.hItem = hSelectedItem;
                        item.mask = TVIF_PARAM;

                        TreeView_GetItem(hwndTree, &item);

                        TreeViewItemParam* param = (TreeViewItemParam*)item.lParam;
                        KPSPropertyView2* psProperty = (KPSPropertyView2*)param->data;

                        psProperty->fileDataSize = fileSize;

                        if (psProperty->fileData)
                            ::free(psProperty->fileData);
                        psProperty->fileData = (unsigned char*)buffer;

                        ::PathStripPathW(fileName); // remove path portion
                        psProperty->fileNameLength = (int)::wcslen(fileName);

                        if (psProperty->fileName)
                            ::free(psProperty->fileName);
                        psProperty->fileName = ::_wcsdup(fileName);

                        psProperty->type = cmbSelectedIndex;

                        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, L"");
                        ::SetWindowTextW(hwndPropertyDlg_Lbl_FileName, psProperty->fileName);
                    }
                    else
                    {
                        ::MessageBoxW(hwndDlg, L"File is empty!", L"Error", MB_ICONERROR);
                    }
                    ::CloseHandle(fileHandle);
                }
                else
                {
                    ::MessageBoxW(hwndDlg, L"Cannot read the file!", L"Error", MB_ICONERROR);
                }
            }

            ::free(fileName);

            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

INT_PTR CALLBACK ObjectDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        hwndObjectDlg_Edit_ObjID = ::GetDlgItem(hwndDlg, IDC_EDIT1);
        hwndObjectDlg_Edit_ObjName = ::GetDlgItem(hwndDlg, IDC_EDIT2); 
        hwndObjectDlg_Btn_Save = ::GetDlgItem(hwndDlg, IDC_BUTTON1);
        return FALSE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON2) // generate button
        {
            GUID objectID;          
            ::CoCreateGuid(&objectID);

            char* objectIDStr;
            ::UuidToStringA(&objectID, (RPC_CSTR*)&objectIDStr);
            ::SetWindowTextA(hwndObjectDlg_Edit_ObjID, objectIDStr);
            ::RpcStringFreeA((RPC_CSTR*)&objectIDStr);

            return TRUE;
        }
        else if (LOWORD(wParam) == IDC_BUTTON1) // update button
        {
            const int objectIDBufferSize = ::GetWindowTextLengthW(hwndObjectDlg_Edit_ObjID);
            if (objectIDBufferSize == 0)
            {
                MessageBoxW(hwndDlg, L"Invalid Object ID!", L"Error", MB_ICONERROR);
                return TRUE;
            }

            wchar_t* objectIDStr = (wchar_t*)::malloc(sizeof(wchar_t) * (objectIDBufferSize+1));
            ::GetWindowTextW(hwndObjectDlg_Edit_ObjID, objectIDStr, objectIDBufferSize+1);

            const int objectIDStrLength = (int)::wcslen(objectIDStr);
            if (objectIDStrLength != 36)
            {
                ::free(objectIDStr);
                MessageBoxW(hwndDlg, L"Invalid Object ID!", L"Error", MB_ICONERROR);
                return TRUE;
            }

            const int objectNameBufferSize = ::GetWindowTextLengthW(hwndObjectDlg_Edit_ObjName);
            if (objectNameBufferSize == 0)
            {
                MessageBoxW(hwndDlg, L"Invalid Object Name!", L"Error", MB_ICONERROR);
                return TRUE;
            }

            wchar_t* objectNameStr = (wchar_t*)::malloc(sizeof(wchar_t) * (objectNameBufferSize + 1));
            ::GetWindowTextW(hwndObjectDlg_Edit_ObjName, objectNameStr, objectNameBufferSize+1);

            const int objectNameStrLength = (int)::wcslen(objectNameStr);
            if (objectNameStrLength < 1)
            {
                ::free(objectNameStr);
                MessageBoxW(hwndDlg, L"Invalid Object Name!", L"Error", MB_ICONERROR);
                return TRUE;
            }

            HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

            TVITEMW item = { 0 };
            item.hItem = hSelectedItem;
            item.mask = TVIF_PARAM;

            TreeView_GetItem(hwndTree, &item);
            
            TreeViewItemParam* param = (TreeViewItemParam*)item.lParam;
            KPSObjectView2* psObject = (KPSObjectView2*)param->data;
            
            if (!psObject->GenerateIDByString(objectIDStr))
            {
                ::free(objectIDStr);
                MessageBoxW(hwndDlg, L"Invalid Object ID!", L"Error", MB_ICONERROR);
                return TRUE;
            }

            ::free(objectIDStr);

            psObject->name = objectNameStr;
            psObject->nameLength = objectNameStrLength;

            TV_ITEMW updateItem = { 0 };
            updateItem.hItem = hSelectedItem;
            updateItem.mask = TVIF_TEXT;
            updateItem.pszText = psObject->name;
            updateItem.cchTextMax = psObject->nameLength;

            ::SendMessageW(hwndTree, TVM_SETITEMW, 0, (LPARAM)&updateItem);

            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

void addNewPSObject(HWND hwndDlg)
{
    ::ShowWindow(hwndPropertyDlg, SW_HIDE);

    KPSObjectView2* psObject = new KPSObjectView2(32);
    psObject->GenerateObjectID();
    psObject->GenerateObjectIDString();
    psObject->name = ::_wcsdup(L"Object Name");
    psObject->nameLength = (int)::wcslen(psObject->name);

    objectList.AddPointer(psObject);

    ::SetWindowTextW(hwndObjectDlg_Edit_ObjID, psObject->objectIDStr);
    ::SetWindowTextW(hwndObjectDlg_Edit_ObjName, psObject->name);

    TV_ITEMW item = {0};
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = psObject->name;
    item.cchTextMax = psObject->nameLength;

    TreeViewItemParam* param = new TreeViewItemParam();
    param->type = 0;
    param->data = psObject;

    psObject->param = param;

    item.lParam = (LPARAM)param;

    TVINSERTSTRUCTW treeViewitem;
    treeViewitem.hParent = NULL;
    treeViewitem.hInsertAfter = TVI_LAST;
    treeViewitem.item = item;

    psObject->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
        0, (LPARAM)&treeViewitem);

    TreeView_SelectItem(hwndTree, psObject->hTree);

    ::ShowWindow(hwndPropertyDlg, SW_HIDE);
    ::ShowWindow(hwndObjectDlg, SW_SHOW);
}

void addProperty(KPSObjectView2* parentObject)
{
    KPSPropertyView2* psProperty = new KPSPropertyView2();
    psProperty->parentObject = parentObject;
    psProperty->name = ::_wcsdup(L"Property Name");
    psProperty->nameLength = (int)::wcslen(psProperty->name);
    psProperty->type = KPSPropertyTypes::INTEGER;
    psProperty->intValue = 0;

    parentObject->propertyList.AddPointer(psProperty);

    ::SetWindowTextW(hwndPropertyDlg_Edit_PropName, psProperty->name);
    ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, L"0");
    ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_SETCURSEL, 1, 0);

    TV_ITEMW item = { 0 };
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = psProperty->name;
    item.cchTextMax = psProperty->nameLength;

    TreeViewItemParam* param = new TreeViewItemParam();
    param->type = 1;
    param->data = psProperty;

    psProperty->param = param;

    item.lParam = (LPARAM)param;

    TVINSERTSTRUCTW treeViewitem;
    treeViewitem.hParent = parentObject->hTree;
    treeViewitem.hInsertAfter = TVI_LAST;
    treeViewitem.item = item;

    psProperty->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
        0, (LPARAM)&treeViewitem);

    TreeView_SelectItem(hwndTree, psProperty->hTree);

    ::ShowWindow(hwndObjectDlg, SW_HIDE);
    ::ShowWindow(hwndPropertyDlg, SW_SHOW);
}

void copyProperty(KPSPropertyView2* srcProperty, KPSPropertyView2* dstProperty)
{
    dstProperty->parentObject = srcProperty->parentObject;
    dstProperty->name = ::_wcsdup(srcProperty->name);
    dstProperty->nameLength = srcProperty->nameLength;
    dstProperty->type = srcProperty->type;

    if (srcProperty->type == KPSPropertyTypes::STRING) // string
    {
        dstProperty->strValue = ::_wcsdup(srcProperty->strValue);
        dstProperty->strValueLength = srcProperty->strValueLength;
    }
    else if (srcProperty->type == KPSPropertyTypes::INTEGER) // int
    {
        dstProperty->intValue = srcProperty->intValue;
    }
    else if (srcProperty->type == KPSPropertyTypes::DWORD) // DWORD
    {
        dstProperty->dwordValue = srcProperty->dwordValue;
    }
    else if (srcProperty->type == KPSPropertyTypes::FLOAT) // float
    {
        dstProperty->floatValue = srcProperty->floatValue;
    }
    else if (srcProperty->type == KPSPropertyTypes::INT_ARRAY) // int array
    {
        dstProperty->intArraySize = srcProperty->intArraySize;
        if (srcProperty->intArraySize)
        {
            dstProperty->intArray = (int*)::malloc(sizeof(int) * srcProperty->intArraySize);
            ::memcpy(dstProperty->intArray, srcProperty->intArray, sizeof(int) * srcProperty->intArraySize);

            dstProperty->GenerateIntArrayString();
        }
    }
    else if (srcProperty->type == KPSPropertyTypes::GUID)// guid
    {
        dstProperty->guidValue = srcProperty->guidValue;
        dstProperty->GenerateGUIDValueString();
    }
    else // file
    {
        dstProperty->fileNameLength = srcProperty->fileNameLength;
        dstProperty->fileName = ::_wcsdup(srcProperty->fileName);

        if (srcProperty->fileDataSize)
        {
            dstProperty->fileData = (unsigned char*)::malloc(srcProperty->fileDataSize);
            ::memcpy(dstProperty->fileData, srcProperty->fileData, srcProperty->fileDataSize);
        }
    }
}

void copyObject(KPSObjectView2* srcObject, KPSObjectView2* dstObject)
{
    dstObject->GenerateObjectID();
    dstObject->GenerateObjectIDString();
    dstObject->name = ::_wcsdup(srcObject->name);
    dstObject->nameLength = srcObject->nameLength;

    // copy the property list
    for (int i = 0; i < srcObject->propertyList.GetSize(); ++i)
    {
        KPSPropertyView2* psProperty = new KPSPropertyView2();
        copyProperty((KPSPropertyView2*)srcObject->propertyList[i], psProperty);
        dstObject->propertyList.AddPointer(psProperty);
    }
}

void UpdatePropertyDialog(KPSPropertyView2* prop)
{
    ::SetWindowTextW(hwndPropertyDlg_Edit_PropName, prop->name);
    ::SendMessageW(hwndPropertyDlg_Cmb_PropType, CB_SETCURSEL, prop->type, 0);

    if (prop->type == KPSPropertyTypes::STRING) // string
    {
        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, prop->strValue);
    }
    else if (prop->type == KPSPropertyTypes::INTEGER) // int
    {
        wchar_t buffer[16];
        ::_itow_s(prop->intValue, buffer, 16, 10);
        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, buffer);
    }
    else if (prop->type == KPSPropertyTypes::DWORD) // DWORD
    {
        wchar_t buffer[16];
        ::wsprintfW(buffer, L"0x%08x", prop->dwordValue);
        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, buffer);
    }
    else if (prop->type == KPSPropertyTypes::FLOAT) // float
    {
        wchar_t buffer[64];
        ::wsprintfW(buffer, L"%.2f", prop->floatValue);
        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, buffer);
    }
    else if (prop->type == KPSPropertyTypes::INT_ARRAY) // int array
    {
        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, prop->intArrayStr);
    }
    else if (prop->type == KPSPropertyTypes::GUID) // guid
    {
        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, prop->guidValueStr);
    }
    else // file
    {
        ::SetWindowTextW(hwndPropertyDlg_Edit_PropValue, L"");
        ::SetWindowTextW(hwndPropertyDlg_Lbl_FileName, prop->fileName);
    }
}

void duplicateProperty(KPSObjectView2* parentObject, KPSPropertyView2* srcProperty)
{
    KPSPropertyView2* psProperty = new KPSPropertyView2();
    copyProperty(srcProperty, psProperty);

    parentObject->propertyList.AddPointer(psProperty);

    UpdatePropertyDialog(psProperty);

    TV_ITEMW item = { 0 };
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = psProperty->name;
    item.cchTextMax = psProperty->nameLength;

    TreeViewItemParam* param = new TreeViewItemParam();
    param->type = 1;
    param->data = psProperty;

    psProperty->param = param;

    item.lParam = (LPARAM)param;

    TVINSERTSTRUCTW treeViewitem;
    treeViewitem.hParent = parentObject->hTree;
    treeViewitem.hInsertAfter = TVI_LAST;
    treeViewitem.item = item;

    psProperty->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
        0, (LPARAM)&treeViewitem);

    TreeView_SelectItem(hwndTree, psProperty->hTree);

    ::ShowWindow(hwndObjectDlg, SW_HIDE);
    ::ShowWindow(hwndPropertyDlg, SW_SHOW);
}

void duplicateObject(KPSObjectView2* srcObject)
{
    KPSObjectView2* psObject = new KPSObjectView2(srcObject->propertyList.GetSize()+10);
    copyObject(srcObject, psObject);

    objectList.AddPointer(psObject);

    ::SetWindowTextW(hwndObjectDlg_Edit_ObjID, psObject->objectIDStr);
    ::SetWindowTextW(hwndObjectDlg_Edit_ObjName, psObject->name);

    TV_ITEMW item = { 0 };
    item.mask = TVIF_TEXT | TVIF_PARAM;
    item.pszText = psObject->name;
    item.cchTextMax = psObject->nameLength;

    TreeViewItemParam* param = new TreeViewItemParam();
    param->type = 0;
    param->data = psObject;

    psObject->param = param;

    item.lParam = (LPARAM)param;

    TVINSERTSTRUCTW treeViewitem;
    treeViewitem.hParent = NULL;
    treeViewitem.hInsertAfter = TVI_LAST;
    treeViewitem.item = item;

    psObject->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
        0, (LPARAM)&treeViewitem);

    // add property items to tree view
    for (int i = 0; i < psObject->propertyList.GetSize(); ++i)
    {
        KPSPropertyView2* psProperty = (KPSPropertyView2*)psObject->propertyList[i];
        TV_ITEMW item = { 0 };
        item.mask = TVIF_TEXT | TVIF_PARAM;
        item.pszText = psProperty->name;
        item.cchTextMax = psProperty->nameLength;

        TreeViewItemParam* param = new TreeViewItemParam();
        param->type = 1;
        param->data = psProperty;

        psProperty->param = param;

        item.lParam = (LPARAM)param;

        TVINSERTSTRUCTW treeViewitem;
        treeViewitem.hParent = psObject->hTree;
        treeViewitem.hInsertAfter = TVI_LAST;
        treeViewitem.item = item;

        psProperty->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
            0, (LPARAM)&treeViewitem);
    }

    TreeView_SelectItem(hwndTree, psObject->hTree);

    ::ShowWindow(hwndPropertyDlg, SW_HIDE);
    ::ShowWindow(hwndObjectDlg, SW_SHOW);
}

void SwapObjects(int object1Index, KPSObjectView2* object1, int object2Index, KPSObjectView2* object2)
{
    // swap objects in objectList
    objectList.SetPointer(object2Index, object1);
    objectList.SetPointer(object1Index, object2);

    KPSObjectView2* topObject = object1Index < object2Index ? object1 : object2;
    KPSObjectView2* bottomObject = object1Index < object2Index ? object2 : object1;

    // remove item
    TreeView_DeleteItem(hwndTree, topObject->hTree);

    // add new item after bottomObject
    TV_ITEMW newItem = { 0 };
    newItem.mask = TVIF_TEXT | TVIF_PARAM;
    newItem.pszText = topObject->name;
    newItem.cchTextMax = topObject->nameLength;
    newItem.lParam = (LPARAM)topObject->param;

    TVINSERTSTRUCTW treeViewitem;
    treeViewitem.hParent = NULL;
    treeViewitem.hInsertAfter = bottomObject->hTree;
    treeViewitem.item = newItem;

    topObject->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
        0, (LPARAM)&treeViewitem);

    // add properties of the object
    for (int i = 0; i < topObject->propertyList.GetSize(); ++i)
    {
        KPSPropertyView2* psProperty = (KPSPropertyView2*)topObject->propertyList[i];

        TV_ITEMW item = { 0 };
        item.mask = TVIF_TEXT | TVIF_PARAM;
        item.pszText = psProperty->name;
        item.cchTextMax = psProperty->nameLength;
        item.lParam = (LPARAM)psProperty->param;

        TVINSERTSTRUCTW treeViewitem;
        treeViewitem.hParent = topObject->hTree;
        treeViewitem.hInsertAfter = TVI_LAST;
        treeViewitem.item = item;

        psProperty->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
            0, (LPARAM)&treeViewitem);
    }
}

void SwapProperties(int property1Index, KPSPropertyView2* property1, int property2Index, 
    KPSPropertyView2* property2, KPSObjectView2* parentObject)
{
    // swap properties in propertyList
    parentObject->propertyList.SetPointer(property2Index, property1);
    parentObject->propertyList.SetPointer(property1Index, property2);

    KPSPropertyView2* topProperty = property1Index < property2Index ? property1 : property2;
    KPSPropertyView2* bottomProperty = property1Index < property2Index ? property2 : property1;

    // remove item
    TreeView_DeleteItem(hwndTree, topProperty->hTree);

    // add new item after bottomProperty
    TV_ITEMW newItem = { 0 };
    newItem.mask = TVIF_TEXT | TVIF_PARAM;
    newItem.pszText = topProperty->name;
    newItem.cchTextMax = topProperty->nameLength;
    newItem.lParam = (LPARAM)topProperty->param;

    TVINSERTSTRUCTW treeViewitem;
    treeViewitem.hParent = parentObject->hTree;
    treeViewitem.hInsertAfter = bottomProperty->hTree;
    treeViewitem.item = newItem;

    topProperty->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
        0, (LPARAM)&treeViewitem);
}

INT_PTR CALLBACK MainDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        hwndTree = ::GetDlgItem(hwndDlg, IDC_TREE1);

        hFontTree = ::CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, 0, 0, DEFAULT_CHARSET, 0, 0, DEFAULT_QUALITY,
            VARIABLE_PITCH | FF_DONTCARE, L"Segoe UI");
        ::SendMessageW(hwndTree, WM_SETFONT, (WPARAM)hFontTree, TRUE);

        RECT treeRect;
        ::GetClientRect(hwndTree, &treeRect);
        ::MapWindowPoints(hwndTree, hwndDlg, (LPPOINT)&treeRect, 2);

        hwndObjectDlg = ::CreateDialogW(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), hwndDlg, ObjectDlgProc);
        ::SetWindowPos(hwndObjectDlg, 0, treeRect.right + 2, 0, 0, 0, SWP_NOSIZE);

        hwndPropertyDlg = ::CreateDialogW(hInstance, MAKEINTRESOURCE(IDD_DIALOG3), hwndDlg, PropertyDlgProc);
        ::SetWindowPos(hwndPropertyDlg, 0, treeRect.right + 2, 0, 0, 0, SWP_NOSIZE);

        ::RegisterHotKey(hwndDlg, W_KEY_ID, MOD_CONTROL | MOD_NOREPEAT, 0x57); // W key
        ::RegisterHotKey(hwndDlg, S_KEY_ID, MOD_CONTROL | MOD_NOREPEAT, 0x53); // s key
        return FALSE;
    }

    case WM_HOTKEY:
    {
        if (wParam == W_KEY_ID) // w key id
        {
            ::PostMessageW(hwndDlg, WM_COMMAND, MAKEWPARAM(ID_EDIT_MOVEUP, 0), 0);
            return FALSE;
        }
        else if (wParam == S_KEY_ID) // s key id
        {
            ::PostMessageW(hwndDlg, WM_COMMAND, MAKEWPARAM(ID_EDIT_MOVEDOWN, 0), 0);
            return FALSE;
        }

        return TRUE;
    }


    case WM_COMMAND:
    {
        if ((HIWORD(wParam) == 0) && (lParam == 0)) // menu item
        {
            if (LOWORD(wParam) == ID_EDIT_ADDNEWOBJECT) // add new object
            {
                addNewPSObject(hwndDlg);
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_EDIT_ADDNEWPROPERTY) // add new property
            {
                HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

                if (hSelectedItem == NULL) // Nothing selected
                    return TRUE;

                TVITEMW item = { 0 };
                item.hItem = hSelectedItem;
                item.mask = TVIF_PARAM;

                if (TreeView_GetItem(hwndTree, &item))
                {
                    TreeViewItemParam* param = (TreeViewItemParam*)item.lParam;
                    KPSObjectView2* parentObject;

                    if (param->type == 0) // object selected
                    {
                        parentObject = (KPSObjectView2*)param->data;
                    }
                    else // property selected. we need to get the parent object.
                    {
                        KPSPropertyView2* psProperty = (KPSPropertyView2*)param->data;
                        parentObject = (KPSObjectView2*)psProperty->parentObject;
                    }

                    addProperty(parentObject);

                    return TRUE;
                }
                return FALSE;
            }
            else if (LOWORD(wParam) == ID_EDIT_DUPLICATE) // duplicate
            {
                HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

                if (hSelectedItem == NULL) // Nothing selected
                    return TRUE;

                TVITEMW item = { 0 };
                item.hItem = hSelectedItem;
                item.mask = TVIF_PARAM;

                if (TreeView_GetItem(hwndTree, &item))
                {
                    TreeViewItemParam* param = (TreeViewItemParam*)item.lParam;

                    if (param->type == 0) // object selected
                    {
                        KPSObjectView2* psObject = (KPSObjectView2*)param->data;
                        duplicateObject(psObject);
                    }
                    else // property selected.
                    {
                        KPSPropertyView2* psProperty = (KPSPropertyView2*)param->data;
                        KPSObjectView2* parentObject = (KPSObjectView2*)psProperty->parentObject;

                        duplicateProperty(parentObject, psProperty);
                    }

                    return TRUE;
                }
                return FALSE;
            }
            else if (LOWORD(wParam) == ID_EDIT_REMOVEOBJECT) // remove selected item
            {

                HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

                if (hSelectedItem == NULL) // Nothing selected
                    return TRUE;

                if (MessageBoxW(hwndDlg, L"Are you sure you want to remove the selected item?", L"Confirm", MB_ICONQUESTION | MB_YESNO) == IDNO)
                    return TRUE;

                TVITEMW item = { 0 };
                item.hItem = hSelectedItem;
                item.mask = TVIF_PARAM;

                if (TreeView_GetItem(hwndTree, &item))
                {
                    TreeViewItemParam* param = (TreeViewItemParam*)item.lParam;

                    // hide both pages. so when there is no more items, nothing will be shown.
                    // if there are anything left, it wil be automatically selected by the system.
                    ::ShowWindow(hwndObjectDlg, SW_HIDE);
                    ::ShowWindow(hwndPropertyDlg, SW_HIDE);

                    TreeView_DeleteItem(hwndTree, hSelectedItem);

                    if (param->type == 0) // object selected
                    {
                        KPSObjectView2* psObject = (KPSObjectView2*)param->data;

                        // remove from object list
                        objectList.RemovePointer(objectList.GetID(psObject));                     

                        delete psObject;
                    }
                    else // property selected.
                    {
                        KPSPropertyView2* psProperty = (KPSPropertyView2*)param->data;
                        KPSObjectView2* parentObject = (KPSObjectView2*)psProperty->parentObject;

                        // remove from parent object property list
                        parentObject->propertyList.RemovePointer(parentObject->propertyList.GetID(psProperty));

                        delete psProperty;
                    }

                    return TRUE;
                }
                return FALSE;
            }
            else if (LOWORD(wParam) == ID_EDIT_MOVEUP) // move up
            {
                HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

                if (hSelectedItem == NULL) // Nothing selected
                    return TRUE;

                TVITEMW srcItem = { 0 };
                srcItem.hItem = hSelectedItem;
                srcItem.mask = TVIF_PARAM;

                if (TreeView_GetItem(hwndTree, &srcItem))
                {
                    TreeViewItemParam* srcParam = (TreeViewItemParam*)srcItem.lParam;

                    if (srcParam->type == 0) // object selected
                    {
                        KPSObjectView2* srcObject = (KPSObjectView2*)srcParam->data;

                        const int srcIndex = objectList.GetID(srcObject);
                        if (srcIndex > 0)
                        {
                            const int dstIndex = srcIndex - 1;
                            KPSObjectView2* dstObject = objectList[dstIndex];

                            SwapObjects(srcIndex, srcObject, dstIndex, dstObject);
                        }
                    }
                    else // property selected.
                    {
                        KPSPropertyView2* srcProperty = (KPSPropertyView2*)srcParam->data;
                        KPSObjectView2* parentObject = (KPSObjectView2*)srcProperty->parentObject;

                        const int srcIndex = parentObject->propertyList.GetID(srcProperty);
                        if (srcIndex > 0)
                        {
                            const int dstIndex = srcIndex - 1;
                            KPSPropertyView2* dstProperty = (KPSPropertyView2*)parentObject->propertyList[dstIndex];

                            SwapProperties(srcIndex, srcProperty, dstIndex, dstProperty, parentObject);
                        }
                    }

                    return TRUE;
                }
                return FALSE; 
            }
            else if (LOWORD(wParam) == ID_EDIT_MOVEDOWN) // move down
            {
                HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

                if (hSelectedItem == NULL) // Nothing selected
                    return TRUE;

                TVITEMW srcItem = { 0 };
                srcItem.hItem = hSelectedItem;
                srcItem.mask = TVIF_PARAM;

                if (TreeView_GetItem(hwndTree, &srcItem))
                {
                    TreeViewItemParam* srcParam = (TreeViewItemParam*)srcItem.lParam;

                    if (srcParam->type == 0) // object selected
                    {
                        KPSObjectView2* srcObject = (KPSObjectView2*)srcParam->data;

                        const int srcIndex = objectList.GetID(srcObject);
                        if (srcIndex < (objectList.GetSize()-1))
                        {
                            const int dstIndex = srcIndex + 1;
                            KPSObjectView2* dstObject = objectList[dstIndex];

                            SwapObjects(srcIndex, srcObject, dstIndex, dstObject);
                            TreeView_SelectItem(hwndTree, srcObject->hTree);
                        }
                    }
                    else // property selected.
                    {
                        KPSPropertyView2* srcProperty = (KPSPropertyView2*)srcParam->data;
                        KPSObjectView2* parentObject = (KPSObjectView2*)srcProperty->parentObject;

                        const int srcIndex = parentObject->propertyList.GetID(srcProperty);
                        if (srcIndex < (parentObject->propertyList.GetSize() - 1))
                        {
                            const int dstIndex = srcIndex + 1;
                            KPSPropertyView2* dstProperty = (KPSPropertyView2*)parentObject->propertyList[dstIndex];

                            SwapProperties(srcIndex, srcProperty, dstIndex, dstProperty, parentObject);
                            TreeView_SelectItem(hwndTree, srcProperty->hTree);
                        }
                    }

                    return TRUE;
                }
                return FALSE;
            }
            else if (LOWORD(wParam) == ID_FILE_SAVEAS) // save as
            {
                if (objectList.GetSize() == 0)
                    return TRUE;

                OPENFILENAMEW ofn;
                ::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

                // assumes MAX_PATH * 2 is enough!
                wchar_t* fileName = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
                fileName[0] = 0;

                ofn.lStructSize = sizeof(OPENFILENAMEW);
                ofn.hwndOwner = hwndDlg;
                ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
                ofn.lpstrFile = fileName;
                ofn.nMaxFile = MAX_PATH * 2;
                ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
                ofn.lpstrTitle = L"Save As...";

                if (::GetSaveFileNameW(&ofn))
                {
                    ::DeleteFileW(fileName);

                    HANDLE fileHandle = ::CreateFileW(fileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

                    if (fileHandle != INVALID_HANDLE_VALUE)
                    {
                        char fileHeader[4] = PS_V1_HEADER;
                        DWORD writtenBytes;
                        ::WriteFile(fileHandle, fileHeader, 4, &writtenBytes, NULL);

                        unsigned int objectCount = objectList.GetSize();
                        ::WriteFile(fileHandle, &objectCount, sizeof(unsigned int), &writtenBytes, NULL);

                        for (unsigned int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
                        {
                            KPSObjectView2* object = objectList[objectIndex];

                            ::WriteFile(fileHandle, &object->objectID, sizeof(GUID), &writtenBytes, NULL);

                            ::WriteFile(fileHandle, &object->nameLength, sizeof(int), &writtenBytes, NULL);
                            ::WriteFile(fileHandle, object->name, sizeof(wchar_t) * object->nameLength, &writtenBytes, NULL);

                            unsigned int propertyCount = object->propertyList.GetSize();
                            ::WriteFile(fileHandle, &propertyCount, sizeof(unsigned int), &writtenBytes, NULL);

                            for (unsigned int propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex)
                            {
                                KPSPropertyView2* psProperty = (KPSPropertyView2*)object->propertyList[propertyIndex];
                                ::WriteFile(fileHandle, &psProperty->nameLength, sizeof(int), &writtenBytes, NULL);
                                ::WriteFile(fileHandle, psProperty->name, sizeof(wchar_t) * psProperty->nameLength, &writtenBytes, NULL);

                                ::WriteFile(fileHandle, &psProperty->type, sizeof(int), &writtenBytes, NULL);

                                if (psProperty->type == KPSPropertyTypes::STRING) // string
                                {
                                    ::WriteFile(fileHandle, &psProperty->strValueLength, sizeof(int), &writtenBytes, NULL);
                                    ::WriteFile(fileHandle, psProperty->strValue, sizeof(wchar_t) * psProperty->strValueLength, &writtenBytes, NULL);
                                }
                                else if (psProperty->type == KPSPropertyTypes::INTEGER) // int
                                {
                                    ::WriteFile(fileHandle, &psProperty->intValue, sizeof(int), &writtenBytes, NULL);
                                }
                                else if (psProperty->type == KPSPropertyTypes::DWORD) // DWORD
                                {
                                    ::WriteFile(fileHandle, &psProperty->dwordValue, sizeof(DWORD), &writtenBytes, NULL);
                                }
                                else if (psProperty->type == KPSPropertyTypes::FLOAT) // float
                                {
                                    ::WriteFile(fileHandle, &psProperty->floatValue, sizeof(float), &writtenBytes, NULL);
                                }
                                else if (psProperty->type == KPSPropertyTypes::INT_ARRAY) // int array
                                {
                                    ::WriteFile(fileHandle, &psProperty->intArraySize, sizeof(int), &writtenBytes, NULL);
                                    ::WriteFile(fileHandle, psProperty->intArray, sizeof(int) * psProperty->intArraySize, &writtenBytes, NULL);
                                }
                                else if(psProperty->type == KPSPropertyTypes::GUID) // guid
                                {
                                    ::WriteFile(fileHandle, &psProperty->guidValue, sizeof(GUID), &writtenBytes, NULL);
                                }
                                else // file
                                {
                                    ::WriteFile(fileHandle, &psProperty->fileNameLength, sizeof(int), &writtenBytes, NULL);
                                    ::WriteFile(fileHandle, psProperty->fileName, sizeof(wchar_t) * psProperty->fileNameLength, &writtenBytes, NULL);
                                    ::WriteFile(fileHandle, &psProperty->fileDataSize, sizeof(DWORD), &writtenBytes, NULL);
                                    ::WriteFile(fileHandle, psProperty->fileData, psProperty->fileDataSize, &writtenBytes, NULL);
                                }
                            }
                        }

                        ::CloseHandle(fileHandle);
                    }
                    else
                    {
                        ::MessageBoxW(hwndDlg, L"Cannot save the file!", L"Error", MB_ICONERROR);
                    }
                }
                ::free(fileName);
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_FILE_OPEN) // open file
            {
                OPENFILENAMEW ofn;
                ::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

                // assumes MAX_PATH * 2 is enough!
                wchar_t* fileName = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
                fileName[0] = 0;

                ofn.lStructSize = sizeof(OPENFILENAMEW);
                ofn.hwndOwner = hwndDlg;
                ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
                ofn.lpstrFile = fileName;
                ofn.nMaxFile = MAX_PATH * 2;
                ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
                ofn.lpstrTitle = L"Open File...";

                if (::GetOpenFileNameW(&ofn))
                {
                    TreeView_DeleteAllItems(hwndTree);
                    objectList.DeleteAll(true);

                    ::ShowWindow(hwndObjectDlg, SW_HIDE);
                    ::ShowWindow(hwndPropertyDlg, SW_HIDE);

                    HANDLE fileHandle = ::CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                    if (fileHandle != INVALID_HANDLE_VALUE)
                    {
                        char fileHeader[4];
                        DWORD bytesRead;
                        ::ReadFile(fileHandle, fileHeader, 4, &bytesRead, NULL);

                        char psFileHeader[4] = PS_V1_HEADER;

                        if ((fileHeader[0] == psFileHeader[0]) && (fileHeader[1] == psFileHeader[1]) && (fileHeader[2] == psFileHeader[2])
                            && (fileHeader[3] == psFileHeader[3]) )
                        {
                            unsigned int objectCount;
                            ::ReadFile(fileHandle, &objectCount, sizeof(unsigned int), &bytesRead, NULL);

                            for (unsigned int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
                            {
                                KPSObjectView2* object = new KPSObjectView2(32);
                                ::ReadFile(fileHandle, &object->objectID, sizeof(GUID), &bytesRead, NULL);

                                object->GenerateObjectIDString();
                                ::ReadFile(fileHandle, &object->nameLength, sizeof(int), &bytesRead, NULL);
                                object->name = (wchar_t*)::malloc(sizeof(wchar_t) * (object->nameLength + 1));
                                ::ReadFile(fileHandle, object->name, sizeof(wchar_t) * object->nameLength, &bytesRead, NULL);
                                object->name[object->nameLength] = 0;

                                objectList.AddPointer(object);

                                TV_ITEMW item = { 0 };
                                item.mask = TVIF_TEXT | TVIF_PARAM;
                                item.pszText = object->name;
                                item.cchTextMax = object->nameLength;

                                TreeViewItemParam* param = new TreeViewItemParam();
                                param->type = 0;
                                param->data = object;

                                object->param = param;
                                item.lParam = (LPARAM)param;

                                TVINSERTSTRUCTW treeViewitem;
                                treeViewitem.hParent = NULL;
                                treeViewitem.hInsertAfter = TVI_LAST;
                                treeViewitem.item = item;

                                object->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
                                    0, (LPARAM)&treeViewitem);

                                unsigned int propertyCount;
                                ::ReadFile(fileHandle, &propertyCount, sizeof(unsigned int), &bytesRead, NULL);

                                for (unsigned int propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex)
                                {
                                    KPSPropertyView2* property = new KPSPropertyView2();
                                    property->parentObject = object;

                                    ::ReadFile(fileHandle, &property->nameLength, sizeof(int), &bytesRead, NULL);
                                    property->name = (wchar_t*)::malloc(sizeof(wchar_t) * (property->nameLength + 1));
                                    ::ReadFile(fileHandle, property->name, sizeof(wchar_t) * property->nameLength, &bytesRead, NULL);
                                    property->name[property->nameLength] = 0;

                                    ::ReadFile(fileHandle, &property->type, sizeof(int), &bytesRead, NULL);

                                    if (property->type == KPSPropertyTypes::STRING) // string
                                    {
                                        ::ReadFile(fileHandle, &property->strValueLength, sizeof(int), &bytesRead, NULL);
                                        property->strValue = (wchar_t*)::malloc(sizeof(wchar_t) * (property->strValueLength + 1));
                                        ::ReadFile(fileHandle, property->strValue, sizeof(wchar_t) * property->strValueLength, &bytesRead, NULL);
                                        property->strValue[property->strValueLength] = 0;
                                    }
                                    else if(property->type == KPSPropertyTypes::INTEGER) // int
                                    {
                                        ::ReadFile(fileHandle, &property->intValue, sizeof(int), &bytesRead, NULL);
                                    }
                                    else if (property->type == KPSPropertyTypes::DWORD) // DWORD
                                    {
                                        ::ReadFile(fileHandle, &property->dwordValue, sizeof(DWORD), &bytesRead, NULL);
                                    }
                                    else if (property->type == KPSPropertyTypes::FLOAT) // float
                                    {
                                        ::ReadFile(fileHandle, &property->floatValue, sizeof(float), &bytesRead, NULL);
                                    }
                                    else if (property->type == KPSPropertyTypes::INT_ARRAY) // int array
                                    {
                                        ::ReadFile(fileHandle, &property->intArraySize, sizeof(int), &bytesRead, NULL);
                                        if (property->intArraySize)
                                        {
                                            property->intArray = (int*)::malloc(sizeof(int) * property->intArraySize);
                                            ::ReadFile(fileHandle, property->intArray, sizeof(int) * property->intArraySize, &bytesRead, NULL);
                                            property->GenerateIntArrayString();
                                        }
                                    }
                                    else if(property->type == KPSPropertyTypes::GUID)// guid
                                    {
                                        ::ReadFile(fileHandle, &property->guidValue, sizeof(GUID), &bytesRead, NULL);
                                        property->GenerateGUIDValueString();
                                    }
                                    else // file
                                    {
                                        ::ReadFile(fileHandle, &property->fileNameLength, sizeof(int), &bytesRead, NULL);
                                        property->fileName = (wchar_t*)::malloc(sizeof(wchar_t) * (property->fileNameLength + 1));
                                        ::ReadFile(fileHandle, property->fileName, sizeof(wchar_t) * property->fileNameLength, &bytesRead, NULL);
                                        property->fileName[property->fileNameLength] = 0;
                                        ::ReadFile(fileHandle, &property->fileDataSize, sizeof(DWORD), &bytesRead, NULL);
                                        if (property->fileDataSize)
                                        {
                                            property->fileData = (unsigned char*)::malloc(property->fileDataSize);
                                            ::ReadFile(fileHandle, property->fileData, property->fileDataSize, &bytesRead, NULL);
                                        }
                                    }

                                    object->propertyList.AddPointer(property);

                                    TV_ITEMW propItem = { 0 };
                                    propItem.mask = TVIF_TEXT | TVIF_PARAM;
                                    propItem.pszText = property->name;
                                    propItem.cchTextMax = property->nameLength;

                                    TreeViewItemParam* param = new TreeViewItemParam();
                                    param->type = 1;
                                    param->data = property;

                                    property->param = param;
                                    propItem.lParam = (LPARAM)param;

                                    TVINSERTSTRUCTW treeViewPropItem;
                                    treeViewPropItem.hParent = object->hTree;
                                    treeViewPropItem.hInsertAfter = TVI_LAST;
                                    treeViewPropItem.item = propItem;

                                    property->hTree = (HTREEITEM)SendMessageW(hwndTree, TVM_INSERTITEMW,
                                        0, (LPARAM)&treeViewPropItem);
                                }
                            }

                            if (objectList.GetSize()) // select the first item
                            {
                                TreeView_SelectItem(hwndTree, objectList[0]->hTree);
                            }
                        }
                        else
                        {
                            ::MessageBoxW(hwndDlg, L"Invalid file!", L"Error", MB_ICONERROR);
                        }
                        ::CloseHandle(fileHandle);
                    }
                    else
                    {
                        ::MessageBoxW(hwndDlg, L"Cannot open the file!", L"Error", MB_ICONERROR);
                    }
                }
                ::free(fileName);
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_FILE_NEWFILE) // new file
            {
                TreeView_DeleteAllItems(hwndTree);

                objectList.DeleteAll(true);

                ::ShowWindow(hwndObjectDlg, SW_HIDE);
                ::ShowWindow(hwndPropertyDlg, SW_HIDE);
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_HELP_ABOUT) // about
            {
                ::MessageBoxW(hwndDlg, L"Property Storage - Editor v1.0\n(c) 2022 CrownSoft\n\nhttps://www.crownsoft.net",
                    L"About", MB_ICONINFORMATION);
                return TRUE;
            }
            else if (LOWORD(wParam) == ID_FILE_EXIT) // exit
            {
                ::PostMessageW(hwndDlg, WM_CLOSE, 0, 0);
                return TRUE;
            }
        }
        return FALSE;
    }

    case WM_NOTIFY:
    {
        LPNM_TREEVIEW pntv = (LPNM_TREEVIEW)lParam;

        if (pntv->hdr.code == TVN_SELCHANGED)
        {
            TreeViewItemParam* param = (TreeViewItemParam*)pntv->itemNew.lParam;
            if (param->type == 0) // user clicked on theme object
            {
                KPSObjectView2* psObject = (KPSObjectView2*)param->data;
                ::SetWindowTextW(hwndObjectDlg_Edit_ObjID, psObject->objectIDStr);
                ::SetWindowTextW(hwndObjectDlg_Edit_ObjName, psObject->name);

                ::ShowWindow(hwndPropertyDlg, SW_HIDE);
                ::ShowWindow(hwndObjectDlg, SW_SHOW);
                return TRUE;
            }
            else if (param->type == 1) // user clicked on property
            {
                KPSPropertyView2* prop = (KPSPropertyView2*)param->data;

                UpdatePropertyDialog(prop);

                ::ShowWindow(hwndObjectDlg, SW_HIDE);
                ::ShowWindow(hwndPropertyDlg, SW_SHOW);        
                return TRUE;
            }
        }
        return FALSE;
    }

    case WM_CLOSE:
        ::UnregisterHotKey(hwndDlg, W_KEY_ID);
        ::UnregisterHotKey(hwndDlg, S_KEY_ID);
        ::DestroyWindow(hwndDlg);
        return TRUE;

    case WM_DESTROY:

        objectList.DeleteAll(false);

        ::DeleteObject(hFontTree);

        PostQuitMessage(0);
        return TRUE;

    default:
        return FALSE;
    }
}

INT WINAPI wWinMain(HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPWSTR lpCmdLine,
    INT nShowCmd)
{
    hInstance = hInst;
    ::InitCommonControls();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    ::DialogBoxW(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, MainDlgProc);

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return 0;
}