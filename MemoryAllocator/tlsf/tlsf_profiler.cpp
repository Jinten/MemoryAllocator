#include"stdafx.h"
#include"tlsf_profiler.h"

using namespace tlsf;
using namespace tlsf::memory;

namespace
{
	template<class T>
	void safe_free(T*& ptr)
	{
		if(ptr)
		{
			std::free(ptr);
			ptr = nullptr;
		}
	}

	template<class T>
	T* memOffset(void* ptr, uint32_t offset)
	{
		return (T*)(((uint8_t*)ptr) + offset);
	};

}

Profiler::Profiler()
{

}

Profiler::~Profiler()
{
	safe_free(mCurrentResult.blockInfos);
}

ProfileResult Profiler::profile(Pool pool)
{
	safe_free(mCurrentResult.blockInfos);

	mCurrentResult.blockCount = pool.mBlockCount;
	mCurrentResult.blockInfos = (BlockInfo*)std::malloc(sizeof(BlockInfo) * pool.mBlockCount);

	auto current = (Pool::Header*)pool.mBuffer;
	for(uint32_t i = 0; i < pool.mBlockCount; ++i)
	{
		auto& blockInfo = mCurrentResult.blockInfos[i];
		blockInfo.used = current->attribute.used;
		blockInfo.fli = current->attribute.fli;
		blockInfo.sli = current->attribute.sli;
		blockInfo.size = current->bodySize;
		blockInfo.addr = memOffset<void>(current, sizeof(Pool::Header));

		current = memOffset<Pool::Header>(current, current->fullSize);
	}

	return mCurrentResult;
}