#include "pch.h"
#include "handler_factory.h"
#include "prop_sheet.h"

HandlerFactory::~HandlerFactory() 
{
	InterlockedDecrement(&objects_count);
}

HandlerFactory::HandlerFactory() : ref_count(1)
{
	InterlockedIncrement(&objects_count);
}

HRESULT __stdcall HandlerFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject)
{
	if (!ppvObject)
		return E_POINTER;
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;
	if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IShellPropSheetExt)) {
		PropSheet* sheet = new PropSheet();
		if (!sheet)
			return E_OUTOFMEMORY;
		HRESULT hr;
		hr = sheet->QueryInterface(riid, ppvObject);
		sheet->Release();
		return hr;
	}
	else
		return E_NOINTERFACE;
}

HRESULT __stdcall HandlerFactory::LockServer(BOOL fLock)
{
	return S_OK;
}

HRESULT __stdcall HandlerFactory::QueryInterface(REFIID riid, void** ppvObject)
{
	if (!ppvObject)
		return E_POINTER;
	*ppvObject = nullptr;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
		*ppvObject = this;
		this->AddRef();
	}
	else
		return E_NOINTERFACE;
	return S_OK;
}

ULONG __stdcall HandlerFactory::AddRef(void)
{
	return InterlockedIncrement(&ref_count);
}

ULONG __stdcall HandlerFactory::Release(void)
{
	DWORD res = InterlockedDecrement(&ref_count);
	if (ref_count < 1) {
		delete this;
	}
	return res;
}
