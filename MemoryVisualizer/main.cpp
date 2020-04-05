#include"stdafx.h"
#include"../MemoryAllocator/tlsf/tlsf_memory.h"

class TestChild;

class TestParent
{
	friend TestChild;
private:
	int mValue;
};

class TestChild
{
	void hoge()
	{
		TestParent a;
		a.mValue = 0;
	}
};

int main(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	tlsf::memory::initialize(util::staticMiB(8), 4);

	{	// need to act Application destructor so nest in this block
		Application app;

		app.setShowImGuiDemoWindow(true);

		tlsf::memory::Profiler profiler;
		auto result = tlsf::memory::profile(profiler);

		app.update([&]()
			{
				// do something...
				ImGui::Begin("Hello, world!");
				ImGui::End();

				auto drawList = ImGui::GetBackgroundDrawList();
				for(u32 i = 0; i < result.blockCount; ++i)
				{
					ImVec2 pos(10, i * 24.0f);

					drawList->AddText(pos, IM_COL32(255, 255, 255, 255), "test");
				}
			});
	}

	tlsf::memory::finalize();

	return 0;
}