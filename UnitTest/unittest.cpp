#include "stdafx.h"
#include "CppUnitTest.h"

#include "../MemoryAllocator/tlsf/tlsf_memory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_MODULE_INITIALIZE(Test_MemoryAllocatorInitialization)
	{
		tlsf::memory::initialize(util::staticMiB(8), 4);
		Logger::WriteMessage("Initialize OK.");
	}
	
	TEST_MODULE_CLEANUP(Test_MemoryAllocatorFinalization)
	{
		tlsf::memory::finalize();
		Logger::WriteMessage("Finalize OK.");
	}

	template<DWORD _ELEM>
	class AllocTester
	{
	public:
		AllocTester()
		{
			// for delay deallocation test.
			mTestData = new TestData();
		}

		~AllocTester()
		{
			delete mTestData;
		}

		static void test()
		{
			auto data = new TestData();
			delete data;
		}
	private:
		struct TestData
		{
			uint8_t value[(DWORD)(_ELEM)];
		};

	private:
		TestData* mTestData = nullptr;
	};

	TEST_CLASS(UnitTest)
	{
	public:
		
		BEGIN_TEST_METHOD_ATTRIBUTE(UnitAllocateTest)
			TEST_PRIORITY(1)
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(UnitAllocateTest)
		{			
			// unit allocate test.
			AllocTester<1<<0>::test();
			AllocTester<1<<1>::test();
			AllocTester<1<<2>::test();
			AllocTester<1<<3>::test();
			AllocTester<1<<4>::test();
			AllocTester<1<<5>::test();
			AllocTester<1<<6>::test();
			AllocTester<1<<7>::test();
			AllocTester<1<<8>::test();
			AllocTester<1<<9>::test();
			AllocTester<1<<10>::test();
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(DelayDeallocateTestForward)
			TEST_PRIORITY(2)
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(DelayDeallocateTestForward)
		{
			AllocTester<1 << 0> test1;
			AllocTester<1 << 1> test2;
			AllocTester<1 << 2> test4;
			AllocTester<1 << 3> test8;
			AllocTester<1 << 4> test16;
			AllocTester<1 << 5> test32;
			AllocTester<1 << 6> test64;
			AllocTester<1 << 7> test128;
			AllocTester<1 << 8> test256;
			AllocTester<1 << 9> test512;
			AllocTester<1 << 10> test1024;
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(DelayDeallocateTestInverse)
			TEST_PRIORITY(3)
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(DelayDeallocateTestInverse)
		{
			AllocTester<1 << 10> test1024;
			AllocTester<1 << 9> test512;
			AllocTester<1 << 8> test256;
			AllocTester<1 << 7> test128;
			AllocTester<1 << 6> test64;
			AllocTester<1 << 5> test32;
			AllocTester<1 << 4> test16;
			AllocTester<1 << 3> test8;
			AllocTester<1 << 2> test4;
			AllocTester<1 << 1> test2;
			AllocTester<1 << 0> test1;
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(DelayDeallocateTestRandom)
			TEST_PRIORITY(4)
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(DelayDeallocateTestRandom)
		{
			AllocTester<1 << 5> test32;
			AllocTester<1 << 4> test16;
			AllocTester<1 << 1> test2;
			AllocTester<1 << 0> test1;
			AllocTester<1 << 3> test8;
			AllocTester<1 << 2> test4;
			AllocTester<1 << 8> test256;
			AllocTester<1 << 10> test1024;
			AllocTester<1 << 9> test512;
			AllocTester<1 << 7> test128;
			AllocTester<1 << 6> test64;
		}
	};
}