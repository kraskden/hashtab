#include "strtool.h"
#include "pch.h"
#include <combaseapi.h>
#include <string>

std::wstring getGuidString(IID iid)
{
	wchar_t* tmp;
	StringFromCLSID(iid, &tmp);
	return std::wstring(tmp);
}

std::wstring getDllName(HMODULE module)
{
	wchar_t path[MAX_PATH];
	GetModuleFileName(module, path, MAX_PATH);
	return std::wstring(path);
}

int getStrSizeInBytes(const std::wstring& str)
{
	return (str.length() + 1) * 2;
}