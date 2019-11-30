#include "pch.h"
#include "prop_sheet.h"
#include "resource.h"

#include <shellapi.h>
#include <prsht.h>

#pragma comment(lib, "comctl32.lib" )

INT_PTR CALLBACK PropPageDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UINT CALLBACK PropPageCallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);

PropSheet::PropSheet() : ref_count(1), data_object(NULL)
{
	InterlockedIncrement(&objects_count);
}

PropSheet::~PropSheet()
{
	InterlockedDecrement(&objects_count);
}

HRESULT __stdcall PropSheet::QueryInterface(REFIID riid, void** ppvObject)
{
	if (!ppvObject)
		return E_POINTER;
	*ppvObject = nullptr;

	if (IsEqualIID(riid, IID_IUnknown)) {
		*ppvObject = this;
		this->AddRef();
	}
	else if (IsEqualIID(riid, IID_IShellPropSheetExt)) {
		*ppvObject = (IShellPropSheetExt*)this;
		this->AddRef();
	}
	else if (IsEqualIID(riid, IID_IShellExtInit)) {
		*ppvObject = (IShellExtInit*)this;
		this->AddRef();
	}
	else
		return E_NOINTERFACE;
	return S_OK;
}

ULONG __stdcall PropSheet::AddRef(void)
{
	return InterlockedIncrement(&ref_count);
}

ULONG __stdcall PropSheet::Release(void)
{
	DWORD res = InterlockedDecrement(&ref_count);
	if (ref_count < 1) {
		delete this;
	}
	return res;
}

HRESULT __stdcall PropSheet::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
	// If Initialize has already been called, release the old
	// IDataObject pointer.
	if (data_object)
	{
		data_object->Release();
	}

	// If a data object pointer was passed in, save it and
	// extract the file name. 
	if (pdtobj)
	{
		data_object = pdtobj;
		data_object->AddRef();

		STGMEDIUM   medium;
		FORMATETC   fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		UINT        uCount;

		if (SUCCEEDED(data_object->GetData(&fe, &medium)))
		{
			// Get the count of files dropped.
			uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

			// Get the first file name from the CF_HDROP.
			if (uCount == 1) {
				DragQueryFile((HDROP)medium.hGlobal, 0, path,
					sizeof(path) / sizeof(TCHAR));
			}

			ReleaseStgMedium(&medium);
		}
	}

	return S_OK;
}


HRESULT __stdcall PropSheet::AddPages(LPFNSVADDPROPSHEETPAGE pfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;

	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_USEREFPARENT | PSP_USETITLE | PSP_DEFAULT |
		PSP_USEICONID  | PSP_USECALLBACK;
	psp.hInstance = dllHandle;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_MEDIUM);
	psp.pszIcon = MAKEINTRESOURCE(IDI_ASTERISK);
	psp.pszTitle = L"hello, andrew!";
	psp.pfnDlgProc = PropPageDlgProc;
	psp.lParam = (LPARAM)path;
	psp.pfnCallback = PropPageCallbackProc;
	psp.pcRefParent = (UINT*)&objects_count;

	hPage = CreatePropertySheetPage(&psp);
	if (hPage)
	{
		if (pfnAddPage(hPage, lParam))
		{
			this->AddRef();
			return S_OK;
		}
		else
		{
			DestroyPropertySheetPage(hPage);
		}
	}
	else
	{
		return E_OUTOFMEMORY;
	}
	return E_FAIL;
	return S_OK;
}

HRESULT __stdcall PropSheet::ReplacePage(EXPPS uPageID, LPFNSVADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam)
{
	return E_NOTIMPL;
}

INT_PTR PropPageDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	return 0;
}

UINT PropPageCallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{

	return 0;
}
