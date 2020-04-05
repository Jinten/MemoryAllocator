#pragma once

#include"tlsf_pool.h"

namespace tlsf
{
	namespace memory
	{
		struct BlockInfo
		{
			bool used = 0;
			uint32_t fli = 0;
			uint32_t sli = 0;
			size_t size = 0;
			void* addr = nullptr;
		};

		struct ProfileResult
		{
			uint32_t blockCount = 0;
			BlockInfo* blockInfos = nullptr;
		};

		class Profiler
		{
		public:
			Profiler();
			~Profiler();

			ProfileResult profile(Pool pool);

		private:
			ProfileResult mCurrentResult;
		};
	}
}