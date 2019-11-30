#include "server.h"
#include "pch.h"

#include "strtool.h"
#include "guid.h"

HRESULT register_server(HMODULE srv)
{
	HKEY hkey;
	DWORD disp;
	LONG res;

#define CREATE_KEY(key, name) \
	res = RegCreateKeyEx((key), (name), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp); \
	if (res != ERROR_SUCCESS) { \
		return E_UNEXPECTED; \
	}

#define SET_KEY(key, prop,  value, size) \
	res = RegSetValueEx((key), (prop), 0, REG_SZ, (BYTE*)(value), (size)); \
	if (res != ERROR_SUCCESS) { \
		return E_UNEXPECTED; \
	}

	std::wstring keyPath;
	keyPath = L"SOFTWARE\\Classes\\CLSID\\" + getGuidString(EXT_GUID);
	// GUID key
	CREATE_KEY(HKEY_LOCAL_MACHINE, keyPath.c_str());
	// Create InProcServer32 key
	CREATE_KEY(hkey, L"InProcServer32");

	// Set server point to dll
	std::wstring dllName = getDllName(srv);
	SET_KEY(hkey, NULL, dllName.c_str(), getStrSizeInBytes(dllName));

	// Create ThreadingModel key and set it to Apartment
	SET_KEY(hkey, L"ThreadingModel", L"Apartment", sizeof L"Apartment");

	// Create ShellExt key and set it
	keyPath = L"SOFTWARE\\Classes\\*\\shellex\\PropertySheetHandlers\\"; 
	keyPath += EXT_NAME;
	CREATE_KEY(HKEY_LOCAL_MACHINE, keyPath.c_str());
	std::wstring guidStr = getGuidString(EXT_GUID);
	int size = getStrSizeInBytes(guidStr);
	SET_KEY(hkey, NULL, guidStr.c_str(), size);

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return S_OK;
}

bool delete_key(const std::wstring& key)
{
	LONG res;
	HKEY hkey;
	bool ret = true;
	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_ALL_ACCESS, &hkey);
	if (res == ERROR_SUCCESS) {
		//MessageBox(NULL, key.c_str(), L"Opened", 0);
		res = RegDeleteKey(HKEY_LOCAL_MACHINE, key.c_str());
		ret = (res == ERROR_SUCCESS);
	}
	RegCloseKey(hkey);
	return ret;
}

HRESULT unregister_server()
{
	std::wstring keyPath = L"SOFTWARE\\Classes\\CLSID\\" + getGuidString(EXT_GUID) + L"\\InProcServer32";
	if (!delete_key(keyPath))
		return E_UNEXPECTED;
	keyPath = L"SOFTWARE\\Classes\\CLSID\\" + getGuidString(EXT_GUID);
	if (!delete_key(keyPath))
		return E_UNEXPECTED;
	
	keyPath = L"SOFTWARE\\Classes\\*\\shellex\\PropertySheetHandlers\\";
	keyPath += EXT_NAME;
	if (!delete_key(keyPath))
		return E_UNEXPECTED;
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return S_OK;
}
