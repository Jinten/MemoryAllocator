#pragma once

#include<Windows.h>
#include<stdint.h>

namespace tlsf
{
	namespace memory
	{
		class Pool
		{
			friend class Profiler;

			union Attribute
			{
				uint32_t value;
				struct
				{
					uint32_t used : 1;
					uint32_t fli : 5;
					uint32_t sli : 5;
					uint32_t reserve : 21;
				};
			};

			struct Header
			{
				Attribute attribute;
				uint32_t fullSize;
				uint32_t bodySize;
				Header* prev;
				Header* next;
			};

			struct Footer
			{
				uint32_t memSize;
			};

		public:
			void initialize(const uint32_t size, const uint32_t divExpIndex);
			void finalize();

			uint8_t* alloc(const size_t size, const uint32_t align);
			void free(void* ptr);

		private:
			uint32_t mSize = 0;
			uint32_t mDivExpIndex = 0;
			uint32_t mBlockCount = 0;
			union
			{
				uint32_t mDividedCount = 0;
				uint32_t mMinimumMemorySize;
			};
			uint32_t mFreeFLIBit;
			uint32_t* mFreeSLIBitList = nullptr;
			Header** mFreeHeaderList = nullptr;
			uint8_t* mBuffer = nullptr;
			uint8_t* mEndOfMemory = nullptr;

		private:
			void registerFreeHeader(Header* header);
			void unregisterFreeHeader(Header* header);
			Header* unregisterFreeHeader(const uint32_t fli, const uint32_t sli);

			uint32_t getFLI(const DWORD size);
			uint32_t getSLI(const DWORD size);

			uint32_t getFreeFLI(const DWORD size, const DWORD freeFLIBit);
			uint32_t getFreeSLI(const DWORD size, const DWORD freeSLIBit, const uint32_t fli);

			DWORD clacBodyMemSize(const uint32_t size)
			{
				return size - (sizeof(Pool::Header) + sizeof(Footer));
			}

			DWORD calcFullMemSize(const uint32_t size)
			{
				return sizeof(Header) + size + sizeof(Footer);
			}
		};
	}
}
