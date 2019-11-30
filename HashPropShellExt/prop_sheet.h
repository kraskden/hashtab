#pragma once

#include "pch.h"

extern DWORD objects_count;
extern HMODULE dllHandle;

class PropSheet : public IShellExtInit, public IShellPropSheetExt
{
protected:
	DWORD ref_count;
	~PropSheet();
	IDataObject *data_object;
	wchar_t path[MAX_PATH];

public:
	PropSheet();

	// Inherited via IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
	virtual ULONG __stdcall AddRef(void) override;
	virtual ULONG __stdcall Release(void) override;

	// Inherited via IShellExtInit
	virtual HRESULT __stdcall Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID) override;

	// Inherited via IShellPropSheetExt
	virtual HRESULT __stdcall AddPages(LPFNSVADDPROPSHEETPAGE pfnAddPage, LPARAM lParam) override;
	virtual HRESULT __stdcall ReplacePage(EXPPS uPageID, LPFNSVADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam) override;

};

