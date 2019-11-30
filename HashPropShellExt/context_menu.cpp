#include "pch.h"
#include "context_menu.h"


ContextMenu::ContextMenu() : ref_count(1)
{
	InterlockedIncrement(&objects_count);
}

ContextMenu::~ContextMenu()
{
	InterlockedDecrement(&objects_count);
}


HRESULT __stdcall ContextMenu::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
	return S_OK;
}

HRESULT __stdcall ContextMenu::QueryInterface(REFIID riid, void** ppvObject)
{
	if (!ppvObject)
		return E_POINTER;
	*ppvObject = nullptr;

	if (IsEqualIID(riid, IID_IUnknown)) {
		*ppvObject = this;
		this->AddRef();
	}
	else if (IsEqualIID(riid, IID_IContextMenu)) {
		*ppvObject = (IContextMenu*)this;
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

ULONG __stdcall ContextMenu::AddRef(void)
{
	return InterlockedIncrement(&ref_count);

}

ULONG __stdcall ContextMenu::Release(void)
{
	DWORD res = InterlockedDecrement(&ref_count);
	if (ref_count < 1) {
		delete this;
	}
	return res;
}

HRESULT __stdcall ContextMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	MENUITEMINFO item = {};
	item.cbSize = sizeof MENUITEMINFO;
	item.fMask = MIIM_STRING | MIIM_ID;
	item.wID = idCmdFirst;
	item.dwTypeData = (LPWSTR)L"CLick me";
	if (!InsertMenuItem(hmenu, 0, TRUE, &item)) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, item.wID - idCmdFirst + 1);
}

HRESULT __stdcall ContextMenu::InvokeCommand(CMINVOKECOMMANDINFO* pici)
{
	system("mspaint.exe");
	return S_OK;
}

HRESULT __stdcall ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax)
{
	return E_NOTIMPL;
}
