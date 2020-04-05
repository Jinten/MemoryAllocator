#pragma once

#include<Windows.h>
#include"tlsf_pool.h"
#include"tlsf_profiler.h"

namespace tlsf
{
	namespace memory
	{
		class Allocator
		{
		public:
			static void initialize(const DWORD memPoolSize, const DWORD expDivIndex);
			static void finalize();

			static void* allocate(const size_t size);
			static void deallocate(void* ptr);

			static ProfileResult profile(Profiler& profiler);
		};
	}
}