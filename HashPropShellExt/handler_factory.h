#pragma once

#include "pch.h"

extern DWORD objects_count;

class HandlerFactory : public IClassFactory, public IUnknown
{
protected:
	DWORD ref_count;
	~HandlerFactory();

public:
	HandlerFactory();

	// Inherited via IClassFactory
	virtual HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override;
	virtual HRESULT __stdcall LockServer(BOOL fLock) override;

	// Inherited via IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
	virtual ULONG __stdcall AddRef(void) override;
	virtual ULONG __stdcall Release(void) override;

};

