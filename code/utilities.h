#pragma once

template<class T>
inline void safe_release(T*& com)
{
	if(com != nullptr)
	{
		com->Release();
		com = nullptr;
	}
}

inline std::wstring format_string(const std::wstring str, ...)
{
	wchar_t temp[1024];
	SecureZeroMemory(temp, sizeof(wchar_t) * 1024);

	va_list args;
	va_start(args, str);
	vswprintf_s(temp, 1024, str.c_str(), args);
	va_end(args);

	return std::wstring(temp);
}

#define ASSERT( expr,str, ... )\
{\
	_ASSERT_EXPR( expr, format_string(str,__VA_ARGS__).c_str() );\
}
