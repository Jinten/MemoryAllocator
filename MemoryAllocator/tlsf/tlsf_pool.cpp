#include"stdafx.h"
#include"tlsf_pool.h"

using namespace tlsf;
using namespace tlsf::memory;

namespace
{
	constexpr uint32_t MAX_DIVIDE_EXP_INDEX = 10;
}


void Pool::initialize(const uint32_t size, const uint32_t divExpIndex)
{
	mDivExpIndex = divExpIndex;
	ASSERT(mDivExpIndex <= MAX_DIVIDE_EXP_INDEX, L"too much specified divide exponential index.");

	mDividedCount = 1 << mDivExpIndex;
	ASSERT(size > mDividedCount, L"must be greater than divided count : 2 ^ mDivExpIndex");

	const uint32_t fullMemSize = sizeof(Header) + size + sizeof(Footer);

	mSize = fullMemSize;
	mBuffer = (uint8_t*)std::malloc(fullMemSize);
	mEndOfMemory = &mBuffer[fullMemSize];

	Header* head = (Header*)mBuffer;
	head->fullSize = fullMemSize;
	head->bodySize = size;
	head->attribute.used = 0;

	mBlockCount = 1;

	Footer* foot = (Footer*)(mBuffer + sizeof(Header) + head->bodySize);
	foot->memSize = head->bodySize;

	const uint32_t fliCount = getFLI(head->bodySize) + 1;
	const uint32_t headerListSize = sizeof(Header*) * (fliCount * mDividedCount);

	mFreeHeaderList = (Header**)std::malloc(headerListSize);
	SecureZeroMemory(mFreeHeaderList, headerListSize);

	mFreeFLIBit = 0;

	mFreeSLIBitList = (uint32_t*)std::malloc(sizeof(uint32_t) * fliCount);
	for(uint32_t i = 0; i < fliCount; ++i)
	{
		mFreeSLIBitList[i] = 0;
	}

	registerFreeHeader(head);
}

void Pool::finalize()
{
	std::free(mBuffer);

	mBuffer = nullptr;
	mEndOfMemory = nullptr;
	mSize = 0;
}

uint8_t* Pool::alloc(const size_t size, const uint32_t align)
{
	// must be greater equal memory size than mDividedCount
	const auto memSize = util::align(max((uint32_t)size, (uint32_t)mMinimumMemorySize), align);

	uint32_t fli = getFreeFLI(memSize, mFreeFLIBit);
	uint32_t sli = getFreeSLI(memSize, mFreeSLIBitList[fli], fli);

	Header* freeHeader = unregisterFreeHeader(fli, sli);

	const uint32_t requestSize = calcFullMemSize(memSize);
	const uint32_t remainFullSize = freeHeader->fullSize - requestSize;

	// able to splite or not
	if(remainFullSize >= calcFullMemSize(mMinimumMemorySize))
	{
		// memory can split
		Header* splitedHeader = util::memOffset<Header>(freeHeader, requestSize);
		splitedHeader->attribute.used = 0;
		splitedHeader->fullSize = remainFullSize;
		splitedHeader->bodySize = clacBodyMemSize(remainFullSize);

		Footer* splitedFooter = util::memOffset<Footer>(splitedHeader, (sizeof(Header) + splitedHeader->bodySize));
		splitedFooter->memSize = splitedHeader->bodySize;

		freeHeader->fullSize = requestSize;
		freeHeader->bodySize = memSize;
		util::memOffset<Footer>(freeHeader, sizeof(Header) + memSize)->memSize = memSize;

		registerFreeHeader(splitedHeader);

		++mBlockCount;
	}

	freeHeader->prev = nullptr;
	freeHeader->next = nullptr;
	freeHeader->attribute.used = 1;
	return util::memOffset<uint8_t>(freeHeader, sizeof(Header));
}

void Pool::free(void* ptr)
{
	Header* header = util::memOffset<Header>(ptr, -int32_t(sizeof(Header)));

	// end of buffer or not
	Header* nextHeader = util::memOffset<Header>(header, header->fullSize);
	if((uint8_t*)nextHeader != mEndOfMemory)
	{
		if(nextHeader->attribute.used == 0)
		{
			// merge memory
			header->fullSize += nextHeader->fullSize;
			header->bodySize += sizeof(Footer) + sizeof(Header) + nextHeader->bodySize; // |Header| --{|Body|Footer| + |Header|Body|}-- |Footer|

			Footer* footer = util::memOffset<Footer>(nextHeader, sizeof(Header) + nextHeader->bodySize);
			footer->memSize = header->bodySize;

			// |~/// unrevealed memory ///| return memory | merge memory |
			unregisterFreeHeader(nextHeader);

			--mBlockCount;
		}
	}

	// head of buffer or not
	if(mBuffer != (uint8_t*)header)
	{
		// check be able to merge previous memory
		Footer* prevFooter = util::memOffset<Footer>(header, -int32_t(sizeof(Footer)));
		Header* prevHeader = util::memOffset<Header>(prevFooter, -int32_t(prevFooter->memSize + sizeof(Header)));

		if(prevHeader->attribute.used == 0)
		{
			// merge
			unregisterFreeHeader(prevHeader);
			--mBlockCount;

			prevHeader->fullSize += header->fullSize;
			prevHeader->bodySize += sizeof(Footer) + sizeof(Header) + header->bodySize;

			Footer* footer = util::memOffset<Footer>(prevHeader, sizeof(Header) + prevHeader->bodySize);
			footer->memSize = prevHeader->bodySize;

			// release that merged prev and next memory block.
			// | return memory | merged memory 1 | merged memory 2|
			registerFreeHeader(prevHeader);
		}
		else
		{
			// release memory as sandwiched using memory blocks.
			// |~/// using memory ///| return memory |/// using memory ///~|
			header->attribute.used = 0;
			registerFreeHeader(header);
		}
	}
	else
	{
		// release memory with merged bottom block.
		// |~/// using memory ///| return memory | merged memory |
		header->attribute.used = 0;
		registerFreeHeader(header);
	}
}

void Pool::registerFreeHeader(Header* header)
{
	header->prev = nullptr;
	header->next = nullptr;

	const uint32_t fli = getFLI(header->bodySize);
	const uint32_t sli = getSLI(header->bodySize);

	mFreeFLIBit |= (1 << fli);
	mFreeSLIBitList[fli] |= (1 << sli);

	header->attribute.fli = fli;
	header->attribute.sli = sli;

	const uint32_t index = fli * mDividedCount + sli;
	if(mFreeHeaderList[index] == nullptr)
	{
		mFreeHeaderList[index] = header;
	}
	else
	{
		Header* current = mFreeHeaderList[index];
		while(current->next)
		{
			current = current->next;
		}

		current->next = header;
		header->prev = current;
	}
}

void Pool::unregisterFreeHeader(Header* header)
{
	const uint32_t offset = header->attribute.fli * mDividedCount;
	const uint32_t index = offset + header->attribute.sli;

	if(header->prev == nullptr)
	{
		if(header->next == nullptr)
		{
			// this seguement is all empty.
			mFreeHeaderList[index] = nullptr;
		}
		else
		{
			// head
			mFreeHeaderList[index] = header->next;
			header->next->prev = nullptr;
			header->next = nullptr;
		}
	}
	else
	{
		if(header->next == nullptr)
		{
			// tail
			header->prev->next = header->next;
			header->prev = nullptr;
		}
		else
		{
			// middle
			header->prev->next = header->next;
			header->next->prev = header->prev;
			header->prev = nullptr;
			header->next = nullptr;
		}
	}

	uint32_t mask = 0;
	for(uint32_t i = 0; i < mDividedCount; ++i)
	{
		if(mFreeHeaderList[offset + i] != nullptr)
		{
			mask |= i;
		}
	}

	mFreeSLIBitList[header->attribute.fli] = mask;
	if(mask == 0)
	{
		mFreeFLIBit &= ~(1 << header->attribute.fli);
	}
}

Pool::Header* Pool::unregisterFreeHeader(const uint32_t fli, const uint32_t sli)
{
	const uint32_t offset = fli * mDividedCount;

	Header* header(nullptr);
	for(uint32_t i = sli; i < mDividedCount; ++i)
	{
		const uint32_t index = offset + i;
		header = mFreeHeaderList[index];

		if(header != nullptr)
		{
			mFreeHeaderList[index] = header->next;
			break;
		}
	}

	uint32_t mask = 0;
	for(uint32_t i = 0; i < mDividedCount; ++i)
	{
		if(mFreeHeaderList[offset + i] != nullptr)
		{
			mask |= i;
		}
	}

	mFreeSLIBitList[fli] = mask;
	if(mask == 0)
	{
		mFreeFLIBit &= ~(1 << fli);
	}

	return header;
}

uint32_t Pool::getFLI(const DWORD size)
{
	const uint32_t index = bin::op::scanReverse(size);
	return index > mDivExpIndex ? index - mDivExpIndex : 0;
}

uint32_t Pool::getSLI(const DWORD size)
{
	// calc top level of bit index
	const DWORD top = bin::op::scanReverse(size);

	// calc right bit shift
	const uint32_t rightShift = top - mDivExpIndex;

	// create mask
	const DWORD mask = bin::op::makeRightMask(top);

	return (size & mask) >> rightShift;
}

uint32_t Pool::getFreeFLI(const DWORD size, const DWORD freeFLIBit) // FLI : First Level Index
{
	const DWORD mask = 0xFFFFFFFFffffffff << bin::op::scanForward(size);
	const DWORD enableFLIBit = (freeFLIBit << mDivExpIndex) & mask;

	ASSERT(enableFLIBit != 0, L"cannot allocate required size. (size = %d)", size);

	return bin::op::scanReverse(enableFLIBit) - mDivExpIndex;
}

uint32_t Pool::getFreeSLI(const DWORD size, const DWORD freeSLIBit, const uint32_t fli) // SLI : Second Level Index
{
	const uint32_t sliMask = (bin::op::makeRightMask(mDivExpIndex) << fli);
	const uint32_t sli = getSLI(size) & sliMask;
	const uint32_t mask = 0xFFFFFFFF << sli;
	const uint32_t enableSLIBit = freeSLIBit & mask;

	if(enableSLIBit == 0)
	{
		return 0xFFFFFFFF;
	}

	return bin::op::scanForward(enableSLIBit);
}