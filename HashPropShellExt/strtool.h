#pragma once
#include <string>
#include "pch.h"

std::wstring getGuidString(IID iid);

std::wstring getDllName(HMODULE module);

int getStrSizeInBytes(const std::wstring& str);