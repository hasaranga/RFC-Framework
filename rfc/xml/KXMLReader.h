
/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>
#include <Xmllite.h>
#include <shlwapi.h>

#define MELEMENT_UNKNOWN -1

// extend KModelElement by each element type.
// each type has unique elementType value which can be used to identify the instance type.
class KModelElement
{
public:
	int elementType;
	KModelElement* next;
	KModelElement* prev;
	KModelElement* parent;
	KModelElement* firstChild;

	KModelElement()
	{
		elementType = MELEMENT_UNKNOWN;
		next = nullptr;
		prev = nullptr;
		parent = nullptr;
		firstChild = nullptr;
	}

	// name and value will become invalid after the call.
	// make a copy of name and value if you are using them for later.
	virtual void setAttribute(const wchar_t* name, UINT nameLength,
		const wchar_t* value, UINT valueLength) {}

	// content will become invalid after the call.
	// make a copy of content if you are using it for later.
	virtual void setContent(const wchar_t* content, UINT length) {}

	// deleting root node will recuresively delete all other nodes.
	virtual ~KModelElement()
	{
		if (firstChild)
			delete firstChild;

		if (next)
			delete next;
	}
};

class KModelElementFactory
{
public:
	// create the required element type with default values.
	// if the elementType is unknown, return an object of KModelElement with id of MELEMENT_UNKNOWN.
	virtual KModelElement* createModelElement(const wchar_t* elementName, UINT length) = 0;
};

// https://learn.microsoft.com/en-us/archive/msdn-magazine/2007/april/xmllite-a-small-and-fast-xml-parser-for-native-c
// requires COM initialization
// builds a custom model when parsing and returns the root element of the model.
class KXMLReader {
protected:
	IStream* stream;

public:
	IXmlReader* reader;

	KXMLReader()
	{
		reader = nullptr;
		stream = nullptr;
		::CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&reader), 0);
	}

	bool loadFromFile(const wchar_t* filePath)
	{
		if (!reader)
			return false;

		if (stream)
			stream->Release();

		stream = nullptr;
		if (::SHCreateStreamOnFileW(filePath, STGM_READ | STGM_SHARE_DENY_WRITE, &stream) != S_OK)
			return false;

		if (reader->SetInput(stream) != S_OK)
		{
			stream->Release();
			stream = nullptr;
			return false;
		}

		return true;
	}

	bool loadFromString(const wchar_t* text, UINT length = 0)
	{
		if (!reader)
			return false;

		if (stream)
		{
			stream->Release();
			stream = nullptr;
		}

		if (length == 0)
		{
			length = (UINT)::wcslen(text);
			if (length == 0)
				return false;
		}

		stream = ::SHCreateMemStream((const BYTE*)text, sizeof(wchar_t) * (length + 1));

		if (!stream)
			return false;

		if (reader->SetInput(stream) != S_OK)
		{
			stream->Release();
			stream = nullptr;
			return false;
		}

		return true;
	}

	// returns the root model element. There can be only one root element in xml.
	KModelElement* parse(KModelElementFactory* factory)
	{
		if ((!reader) || (!stream) || (!factory))
			return nullptr;

		KModelElement* rootElement = nullptr;
		KModelElement* lastElement = nullptr;
		KModelElement* lastElementWhichExpectedChild = nullptr;

		XmlNodeType nodeType = XmlNodeType_None;

		bool expectChild = false;
		while (S_OK == reader->Read(&nodeType))
		{
			switch (nodeType)
			{
				case XmlNodeType_Element:
				{
					PCWSTR elementName = nullptr;
					UINT elementNameLength = 0;

					// localName is valid before calling MoveToFirstAttribute
					reader->GetLocalName(&elementName, &elementNameLength);

					bool selfClosingElement = reader->IsEmptyElement(); // call before MoveToFirstAttribute

					KModelElement* newElement = factory->createModelElement(elementName, elementNameLength);
					if (!rootElement)
						rootElement = newElement;

					if (expectChild)
					{
						newElement->parent = lastElement;
						if (!lastElement->firstChild)
							lastElement->firstChild = newElement;
					}
					else
					{
						if (lastElement)
						{
							newElement->parent = lastElement->parent;
							newElement->prev = lastElement;
							lastElement->next = newElement;
						}
					}

					PCWSTR attribName = nullptr;
					PCWSTR attribValue = nullptr;
					UINT attribNameLength = 0;
					UINT attribValueLength = 0;

					for (HRESULT result = reader->MoveToFirstAttribute();
						S_OK == result;
						result = reader->MoveToNextAttribute())
					{
						reader->GetLocalName(&attribName, &attribNameLength);
						reader->GetValue(&attribValue, &attribValueLength);

						newElement->setAttribute(attribName, attribNameLength,
							attribValue, attribValueLength);
					}

					if (selfClosingElement)
					{
						expectChild = false;
					}
					else
					{
						expectChild = true;
						lastElementWhichExpectedChild = newElement;
					}

					lastElement = newElement;

					break;
				}
				case XmlNodeType_Text:
				{
					if (lastElement)
					{
						UINT elementContentLength = 0;
						PCWSTR elementContent = nullptr;
						reader->GetValue(&elementContent, &elementContentLength);
						lastElement->setContent(elementContent, elementContentLength);
					}

					break;
				}
				case XmlNodeType_EndElement:
				{
					expectChild = false;
					if (lastElementWhichExpectedChild)
					{
						lastElement = lastElementWhichExpectedChild;
						lastElementWhichExpectedChild = nullptr;
					}
					else
					{
						if (lastElement)
							lastElement = lastElement->parent;
					}
					break;
				}
				default:
					break;
			}
		}

		return rootElement;
	}

	virtual ~KXMLReader()
	{
		if (stream)
			stream->Release();

		if (reader)
			reader->Release();
	}
};


#ifdef _TEST_KXML_READER_

#include <wchar.h>

#define MELEMENT_LABEL 100
#define MELEMENT_PANEL 101

class LabelElement : public KModelElement
{
public:
	wchar_t* text;
	int x;
	int y;

	LabelElement()
	{
		elementType = MELEMENT_LABEL;
		text = ::_wcsdup(L"Default Text");
		x = 0;
		y = 0;
	}

	void setAttribute(const wchar_t* name, UINT nameLength,
		const wchar_t* value, UINT valueLength) override
	{
		::wprintf(L"element=Label attribute=%s value=%s\n", name, value);

		if (::wcscmp(name, L"x") == 0)
			x = ::_wtoi(value);
		else if (::wcscmp(name, L"y") == 0)
			y = ::_wtoi(value);
	}

	void setContent(const wchar_t* content, UINT length) override
	{
		::wprintf(L"element=Label content=%s\n", content);

		if (length)
		{
			if (text)
				::free(text);
			text = ::_wcsdup(content);
		}
	}

	~LabelElement()
	{
		::wprintf(L"deleting Label\n");
		if (text)
			::free(text);
	}
};

class PanelElement : public KModelElement
{
public:
	int width;
	int height;

	PanelElement()
	{
		elementType = MELEMENT_PANEL;
		width = 100;
		height = 100;
	}

	void setAttribute(const wchar_t* name, UINT nameLength,
		const wchar_t* value, UINT valueLength) override
	{
		::wprintf(L"element=Panel attribute=%s value=%s\n", name, value);

		if (::wcscmp(name, L"width") == 0)
			width = ::_wtoi(value);
		else if (::wcscmp(name, L"height") == 0)
			height = ::_wtoi(value);
	}

	~PanelElement()
	{
		::wprintf(L"deleting Panel\n");
	}
};

class TestModelElementFactory : public KModelElementFactory
{
public:
	KModelElement* createModelElement(const wchar_t* elementName, UINT length) override
	{
		if (::wcscmp(elementName, L"Label") == 0)
		{
			LabelElement* element = new LabelElement();
			return element;
		}
		else if (::wcscmp(elementName, L"Panel") == 0)
		{
			PanelElement* element = new PanelElement();
			return element;
		}
		else // unknown element
		{
			return new KModelElement();
		}
	}
};

class TestClass
{
	int tabCount;

	const wchar_t* getElementName(KModelElement* element)
	{
		switch (element->elementType)
		{
		case MELEMENT_PANEL:
			return L"Panel";
		case MELEMENT_LABEL:
			return L"Label";
		default:
			return L"Unknown";
		}
	}

	void printElements(KModelElement* element)
	{
		int count = tabCount;
		while (count)
		{
			::wprintf(L"\t");
			--count;
		}

		::wprintf(getElementName(element));

		if (element->parent)
			::wprintf(L" parent:%s ", getElementName(element->parent));

		if (element->next)
			wprintf(L" next:%s ", getElementName(element->next));

		if (element->prev)
			wprintf(L" prev:%s ", getElementName(element->prev));

		if (element->firstChild)
			wprintf(L" firstChild:%s ", getElementName(element->firstChild));

		wprintf(L"\n");

		if (element->firstChild)
		{
			tabCount++;
			printElements(element->firstChild);
			tabCount--;
		}

		if (element->next)
		{
			printElements(element->next);
		}

	}

public:
	TestClass()
	{
		tabCount = 0;
	}

	void TestFunction()
	{
		const wchar_t* xmlText = L"<Panel width = '400' height = '200'>"
			"<Panel width = '400' height = '100'>"
			"<Label x='10' y='10'>My Label1</Label>"
			"</Panel><Panel width = '400' height = '100'>"
			"<Label x='10' y='10'>"
			"<Label x='0' y='0'>My Label2</Label>"
			"<Label x='5' y='5'>My Label3</Label>"
			"</Label></Panel></Panel>";

		TestModelElementFactory factory;
		KXMLReader xmlReader;

		if (xmlReader.loadFromString(xmlText))
		{
			KModelElement* rootElement = xmlReader.parse(&factory);
			if (rootElement)
			{
				::wprintf(L"\n===========================\n\n");
				printElements(rootElement);
				::wprintf(L"\n===========================\n\n");
				delete rootElement;
			}
		}
	}
};

#endif