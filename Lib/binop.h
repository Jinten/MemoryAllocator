#pragma once

#include<Windows.h>
#include<stdint.h>

namespace bin
{
	namespace op
	{
		template<class T> 
		uint32_t scanForward(const T mask)
		{
			DWORD index;
			_BitScanForward(&index, mask);

			return (uint32_t)index;
		}

		template<class T>
		uint32_t scanReverse(const T mask)
		{
			DWORD index;
			_BitScanReverse(&index, mask);

			return (uint32_t)index;
		}

		inline DWORD makeMask(const uint32_t index)
		{
			return ~(0xFFFFFFFFffffffff << index);
		}

		inline DWORD makeRightMask(const uint32_t index)
		{
			return (1 << index) - 1;
		}
	}
}