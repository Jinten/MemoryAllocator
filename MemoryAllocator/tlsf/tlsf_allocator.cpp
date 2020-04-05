#include"stdafx.h"
#include"tlsf_allocator.h"

using namespace tlsf;
using namespace tlsf::memory;

namespace
{
	bool Initialized = false;
	Pool MemoryPool;
}


void Allocator::initialize(const DWORD memPoolSize, const DWORD divExpIndex)
{
	Initialized = true;
	MemoryPool.initialize(memPoolSize, divExpIndex);
}

void Allocator::finalize()
{
	MemoryPool.finalize();
}

void* Allocator::allocate(const size_t size)
{
	if(Initialized)
	{
		return MemoryPool.alloc(size, 4);
	}

	return std::malloc(size);
}

void Allocator::deallocate(void* ptr)
{
	if(Initialized)
	{
		MemoryPool.free(ptr);
		return;
	}

	std::free(ptr);
}

ProfileResult Allocator::profile(Profiler& profiler)
{
	return profiler.profile(MemoryPool);
}