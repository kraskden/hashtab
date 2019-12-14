#pragma once

#include "pch.h"
#include <string>
#include <cryptlib.h>

extern DWORD objects_count;
extern HMODULE dllHandle;

struct SharedInfo {
	wchar_t path[MAX_PATH];
	HANDLE handle;
	bool is_custom;
	wchar_t custom_name[256];
	wchar_t hash[256];
	wchar_t md5[256];
	wchar_t sha1[256];
	wchar_t crc32[256];
};

class PropSheet : public IShellExtInit, public IShellPropSheetExt
{
protected:
	DWORD ref_count;
	~PropSheet();
	IDataObject *data_object;
	SharedInfo* sharedInfo = new SharedInfo();

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

INT_PTR CALLBACK PropPageDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UINT CALLBACK PropPageCallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);
void init_page(HWND hwnd, PROPSHEETPAGE* page);
void on_timer(HWND hwnd, PROPSHEETPAGE* page);
bool getHash(WCHAR* path, std::wstring& md5_str, std::wstring& sha1_str, std::wstring& crc32_str);
std::wstring hashToString(CryptoPP::HashTransformation* hash);
void compare_hash(HWND hwnd);
void init_custom_hash(HWND hwnd, PROPSHEETPAGE* page);
void thread_start(HWND hwnd, SharedInfo* info);
DWORD WINAPI thread_compute_custom(VOID* in);
bool getCustomHash(WCHAR* path, std::wstring& res, CryptoPP::HashTransformation* hash);
CryptoPP::HashTransformation* getHashProvider(const std::wstring& name);



