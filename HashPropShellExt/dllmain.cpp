// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "server.h"
#include "guid.h"
#include "handler_factory.h"

HMODULE dllHandle;
DWORD objects_count;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		dllHandle = hModule;
		objects_count = 0;
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HRESULT __stdcall DllRegisterServer(void) 
{
	return register_server(dllHandle);
}

HRESULT __stdcall DllUnregisterServer(void) 
{
	return unregister_server();
}

HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	if (!IsEqualCLSID(rclsid, EXT_GUID))
		return CLASS_E_NOAGGREGATION;
	if (!ppv)
		return E_POINTER;
	HandlerFactory* factory = new HandlerFactory();
	if (!factory)
		return E_OUTOFMEMORY;
	HRESULT hr;
	hr = factory->QueryInterface(riid, ppv);
	factory->Release();
	return hr;

	return S_OK;
}

HRESULT __stdcall DllCanUnloadNow(void)
{
	return objects_count == 0 ? S_OK : S_FALSE;
}
