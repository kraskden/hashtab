#pragma once

#include "pch.h"

extern DWORD objects_count;

class ContextMenu : public IUnknown, public IShellExtInit, public IContextMenu
{
protected:
	DWORD ref_count;
	~ContextMenu();

public:
	ContextMenu();

	// Inherited via IShellExtInit
	virtual HRESULT __stdcall Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID) override;

	// Inherited via IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
	virtual ULONG __stdcall AddRef(void) override;
	virtual ULONG __stdcall Release(void) override;

	// Inherited via IContextMenu
	virtual HRESULT __stdcall QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) override;
	virtual HRESULT __stdcall InvokeCommand(CMINVOKECOMMANDINFO* pici) override;
	virtual HRESULT __stdcall GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) override;

};

