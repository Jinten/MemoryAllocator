#pragma once

#include<iostream>
#include<cstddef>
#include<Windows.h>
#include"tlsf_allocator.h"

namespace tlsf
{
	namespace memory
	{
		void initialize(uint32_t memPoolSize, uint32_t divExpIndex)
		{
			tlsf::memory::Allocator::initialize(memPoolSize, divExpIndex);
		}

		inline void finalize()
		{
			tlsf::memory::Allocator::finalize();
		}

		inline void* allocate(size_t size)
		{
			return tlsf::memory::Allocator::allocate(size);
		}

		inline void* allocate(size_t size, const char* file, const int line)
		{
			return tlsf::memory::Allocator::allocate(size);
		}

		inline void deallocate(void* ptr, const size_t size)
		{
			tlsf::memory::Allocator::deallocate(ptr);
		}

		inline ProfileResult profile(Profiler& profiler)
		{
			return tlsf::memory::Allocator::profile(profiler);
		}
	}
}

void* operator new(const size_t size)
{
	return tlsf::memory::allocate(size);
}

void* operator new(const size_t size, const char* file, const int line)
{
	return tlsf::memory::allocate(size, file, line);
}

void operator delete(void* const ptr, const size_t size)
{
	tlsf::memory::deallocate(ptr, size);
}

#if _TEST
#define new ::new(__FILE__, __LINE__)
#endif

#pragma comment(lib,"MemoryAllocator.lib")