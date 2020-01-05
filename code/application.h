#pragma once

#include"window.h"
#include"../imgui/imgui.h"
#include"../imgui/examples/imgui_impl_win32.h"
#include"../imgui/examples/imgui_impl_dx11.h"

class Application
{
public:
	Application();
	Application(Window::Desc& desc);
	~Application();

	void setBackgroundColor(const ImVec4& color)
	{
		mBackgroundColor = color;
	}

	void setShowImGuiDemoWindow(bool enable)
	{
		mShowImGuiDemoWindow = enable;
	}

	void update(Action action);

private:
	bool mKilled = false;;
	bool mShowImGuiDemoWindow = false;
	ImVec4 mBackgroundColor;

	Window mWindow;

private:
	static void updateMessage(Window::MessageInfo* info);
};