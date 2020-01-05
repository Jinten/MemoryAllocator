#include"stdafx.h"
#include"application.h"

int main(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	Application app;

	app.setShowImGuiDemoWindow(true);

	app.update([&]()
		{
			// do something...
			ImGui::Begin("Hello, world!");
			ImGui::End();
		});

	return 0;
}