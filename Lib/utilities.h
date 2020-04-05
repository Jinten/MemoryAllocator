#pragma once

#include<Windows.h>

namespace util
{
	template<class T>
	inline void safe_release(T*& com)
	{
		if(com != nullptr)
		{
			com->Release();
			com = nullptr;
		}
	}

	template<class T>
	T* memOffset(void* ptr, int32_t offset)
	{
		return (T*)(((uint8_t*)ptr) + offset);
	}

	inline constexpr uint32_t staticKiB(const uint32_t size)
	{
		return size << 10;
	}

	inline constexpr uint32_t staticMiB(const uint32_t size)
	{
		return size << 20;
	}

	inline constexpr uint32_t staticAlign(const uint32_t N, const uint32_t ALIGH)
	{
		return (((N)+(ALIGH))&(~(ALIGH - 1)));
	}

	inline DWORD align(const uint32_t n, const uint32_t align)
	{
		return (n + (align - 1)) & (~(align - 1));
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
}

#define ASSERT( expr,str, ... )\
{\
	_ASSERT_EXPR( expr, util::format_string(str,__VA_ARGS__).c_str() );\
}
