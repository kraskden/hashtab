#include "pch.h"
#include "prop_sheet.h"
#include "resource.h"

#include <shellapi.h>
#include <prsht.h>

#include <string>
#include <map>
#include <set>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptlib.h>
#include <sha.h>
#include <md5.h>
#include <hex.h>
#include <crc.h>

#pragma comment(lib, "comctl32.lib" )
#pragma comment(lib, "cryptlib.lib" )

#define IDT_TIMER 1

std::map<HWND, PROPSHEETPAGE*> hwnd_map;

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
	if (res < 1) {
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
				//DragQueryFile((HDROP)medium.hGlobal, 0, path,
				//	MAX_PATH);
				DragQueryFile((HDROP)medium.hGlobal, 0, sharedInfo->path,
					MAX_PATH);
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
	psp.pszTemplate = MAKEINTRESOURCE(IDD_MYPROP);
	psp.pszIcon = MAKEINTRESOURCE(IDI_ASTERISK);
	psp.pszTitle = L"Hash";
	psp.pfnDlgProc = PropPageDlgProc;
	psp.lParam = (LPARAM)sharedInfo;
	psp.pfnCallback = PropPageCallbackProc;
	psp.pcRefParent = (UINT*)&objects_count;

	hPage = CreatePropertySheetPage(&psp);
	if (hPage)
	{
		if (pfnAddPage(hPage, lParam))
		{
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
}

HRESULT __stdcall PropSheet::ReplacePage(EXPPS uPageID, LPFNSVADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam)
{
	return E_NOTIMPL;
}

INT_PTR PropPageDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		init_page(hwnd, (PROPSHEETPAGE*)lParam);
		hwnd_map[hwnd] = (PROPSHEETPAGE*)lParam;
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_BUTTONCALC) { // Custom hash
			MessageBoxA(NULL, "", "", 0);
		}
		if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_EDITENTER) {
			compare_hash(hwnd);
		}
		break;

	case WM_TIMER:
		on_timer(hwnd, hwnd_map[hwnd]);
	}
	return 0;
}

DWORD WINAPI thread_compute(VOID* t);

void render_hash(HWND hwnd, const wchar_t* crc, const wchar_t* md5, const wchar_t* sha1)
{
	SetDlgItemText(hwnd, IDC_EDITCRC, crc);
	SetDlgItemText(hwnd, IDC_EDITMD5, md5);
	SetDlgItemText(hwnd, IDC_EDITSHA1, sha1);
}

void compare_hash(HWND hwnd)
{
	std::set<std::wstring> correct_hashes;
	wchar_t buff[4096];

#define ADD_ITEM(ID) \
	GetDlgItemText(hwnd, (ID), buff, sizeof(buff) / sizeof(wchar_t)); \
	if (*buff) \
		correct_hashes.insert(std::wstring(buff))

	ADD_ITEM(IDC_EDITCRC);
	ADD_ITEM(IDC_EDITCUSTOM);
	ADD_ITEM(IDC_EDITMD5);
	ADD_ITEM(IDC_EDITSHA1);

#undef ADD_ITEM

	GetDlgItemText(hwnd, IDC_EDITENTER, buff, sizeof(buff) / sizeof(wchar_t));
	std::wstring curr_hash(buff);

	if (correct_hashes.find(curr_hash) != correct_hashes.end()) {
		SetDlgItemText(hwnd, IDC_CORRECT, L"CORRECT");
	}
	else {
		SetDlgItemText(hwnd, IDC_CORRECT, L"INCORRECT");
	}

}


void init_page(HWND hwnd, PROPSHEETPAGE* page)
{
	HWND combobox = GetDlgItem(hwnd, IDC_COMBOALGO);
	SendMessage(combobox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"SHA256");
	SendMessage(combobox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"SHA512");
	SendMessage(combobox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	SharedInfo* info = (SharedInfo*)page->lParam;
	//SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
	HANDLE thread = CreateThread(NULL, 0, thread_compute, (LPVOID)page->lParam, 0, NULL);
	SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST);
	info->handle = thread;
	if (WaitForSingleObject(thread, 200) == WAIT_TIMEOUT) { // Long hash calculations
		render_hash(hwnd, L"Computing...", L"Computing...", L"Computing...");
		SetTimer(hwnd, IDT_TIMER, 200, NULL);
	}
	else {
		render_hash(hwnd, info->crc32, info->md5, info->sha1);
	}
}

void on_timer(HWND hwnd, PROPSHEETPAGE* page)
{
	SharedInfo* info = (SharedInfo*)page->lParam;
	if (WaitForSingleObject(info->handle, 0) == WAIT_OBJECT_0) {
		render_hash(hwnd, info->crc32, info->md5, info->sha1);
		KillTimer(hwnd, IDT_TIMER);
	}
}

DWORD WINAPI thread_compute(VOID* in)
{
	SharedInfo* info = (SharedInfo*)in;
	std::wstring md5, sha1, crc32;

	getHash(info->path, md5, sha1, crc32);
	wcscpy(info->md5, md5.c_str());
	wcscpy(info->sha1, sha1.c_str());
	wcscpy(info->crc32, crc32.c_str());
	return 0;
}

bool getHash(WCHAR* path, std::wstring& md5_str, std::wstring& sha1_str, std::wstring& crc32_str)
{
	using namespace CryptoPP;
	Weak::MD5 md5; SHA1 sha1; CRC32 crc32;
	HashTransformation* hash_transform[] = { &md5, &crc32, &sha1 };
	HANDLE hFile = CreateFile(path,                   // lpFileName
		GENERIC_READ,               // dwDesiredAccess
		0,                          // dwShareMode
		NULL,                       // lpSecurityAttributes
		OPEN_EXISTING,              // dwCreationDisposition
		FILE_FLAG_BACKUP_SEMANTICS, // dwFlagsAndAttributes
		NULL);                     // hTemplateFile
	if (hFile) {
		DWORD count;
		constexpr int size = 1024 * 1024;
		UINT8* buff = new UINT8[size];
		while (1) {
			ReadFile(hFile, buff, size, &count, NULL);
			if (count == 0)
				break;
			for (auto h : hash_transform) {
				h->Update(buff, count);
			}
		}
		delete buff;
		CloseHandle(hFile);

		md5_str = hashToString(&md5);
		sha1_str = hashToString(&sha1);
		crc32_str = hashToString(&crc32);
	}
	else {
		return false;
	}
	return true;

}

std::wstring hashToString(CryptoPP::HashTransformation* hash)
{
	byte* res = new byte[hash->DigestSize()];
	hash->Final(res);
	CryptoPP::HexEncoder encoder;
	encoder.Put(res, hash->DigestSize());
	encoder.MessageEnd();
	std::string encoded;
	encoded.resize(encoder.MaxRetrievable());
	encoder.Get((byte*)&encoded[0], encoded.size());
	delete res;

	return std::wstring(encoded.begin(), encoded.end());
}

CryptoPP::HashTransformation* getHashProvider(const std::wstring& name)
{
	if (name == L"SHA256")
		return new CryptoPP::SHA256();
	else if (name == L"SHA512")
		return new CryptoPP::SHA512();
	else
		return NULL;
}


UINT CALLBACK PropPageCallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
	if (PSPCB_RELEASE == uMsg)
	{
		delete (SharedInfo*)ppsp->lParam;
	}
	return 1;
}

