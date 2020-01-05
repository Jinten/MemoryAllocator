#include"stdafx.h"
#include"window.h"


Window::Window()
{

}

Window::~Window()
{
	::DestroyWindow(mInfo.hWnd);
	::UnregisterClass(mInfo.regName.c_str(), mInfo.winEx.hInstance);
}

void Window::create(const Desc& desc)
{
	::SecureZeroMemory(&mInfo.msg, sizeof(mInfo.msg));
	::SecureZeroMemory(&mInfo.winEx, sizeof(mInfo.winEx));

	registerInstance(desc.hInstance ? desc.hInstance : (HINSTANCE)GetModuleHandle(0));

	createInternal(desc);
	mInfo.clientData = desc.clientData;
	mInfo.isParent = (mInfo.hWndParent == nullptr) ? true : false;
	mInfo.isWindowed = !desc.fullScreenEnable;

	::SetWindowLongPtr(mInfo.hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	::UpdateWindow(mInfo.hWnd);
}

void Window::updateMessage()
{
	if(PeekMessage(&mInfo.msg, mInfo.hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&mInfo.msg);
		DispatchMessage(&mInfo.msg);
	}
}

LRESULT	CALLBACK Window::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = (Window*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(msg)
	{
	case WM_SIZE:
	case WM_SIZING:
	{
		RECT rect;
		::GetWindowRect(window->mInfo.hWnd, &rect);
		window->mInfo.width = rect.right - rect.left;
		window->mInfo.height = rect.bottom - rect.top;

		::GetClientRect(window->mInfo.hWnd, &rect);
		window->mInfo.clientWidth = rect.right - rect.left;
		window->mInfo.clientHeight = rect.bottom - rect.top;
		break;
	}
	}

	if(window)
	{
		MessageInfo info(hWnd, msg, wParam, lParam, window->mInfo.clientData, false);

		window->OnMessage.invoke(&info);

		return info.handled ? true : DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::updateRect(u32 width, u32 height, DWORD style)
{
	RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

	::AdjustWindowRect(&rc, style, FALSE);

	mInfo.width = rc.right - rc.left;
	mInfo.height = rc.bottom - rc.top;
	mInfo.clientWidth = width;
	mInfo.clientHeight = height;
}

void Window::registerInstance(const HINSTANCE hInstance)
{
	mInfo.regName.assign(L"sandbox_window");

	mInfo.winEx.cbSize = sizeof(WNDCLASSEX);
	mInfo.winEx.style = CS_HREDRAW | CS_VREDRAW;
	mInfo.winEx.hInstance = hInstance;
	mInfo.winEx.lpszClassName = mInfo.regName.c_str();
	mInfo.winEx.lpfnWndProc = (WNDPROC)Window::wndProc;
	mInfo.winEx.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	mInfo.winEx.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	mInfo.winEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	mInfo.winEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	mInfo.winEx.lpszMenuName = nullptr;
	mInfo.winEx.cbClsExtra = 0;
	mInfo.winEx.cbWndExtra = 0;

	bool result = !!RegisterClassEx(&mInfo.winEx);
	ASSERT(result, L"RegisterClassExA has failed.");
}

void Window::createInternal(const Desc& desc)
{
	updateRect(desc.width, desc.height, desc.style);

	mInfo.style = desc.style;
	mInfo.hWndParent = desc.hWndParent;
	mInfo.caption.assign(desc.caption);

	POINT showPos;
	if(desc.autoCalcShowPos)
	{
		showPos.x = static_cast<LONG>((GetSystemMetrics(SM_CXSCREEN) - (desc.width)) * 0.5f);
		showPos.y = static_cast<LONG>((GetSystemMetrics(SM_CYSCREEN) - (desc.height)) * 0.5f);
	}
	else
	{
		showPos.x = desc.showPos.x;
		showPos.y = desc.showPos.y;
	}

	HINSTANCE hInstance = desc.hInstance ? desc.hInstance : (HINSTANCE)GetModuleHandle(0);

	mInfo.hWnd = ::CreateWindowEx(
		WS_EX_LAYERED,
		mInfo.regName.c_str(),
		mInfo.caption.c_str(),
		mInfo.style,
		showPos.x,
		showPos.y,
		mInfo.width,
		mInfo.height,
		desc.hWndParent,
		nullptr,
		hInstance,
		this);

	ASSERT(mInfo.hWnd != nullptr, L"Failed to create window.");

	//to be not transparent.
	SetLayeredWindowAttributes(mInfo.hWnd, 50, 255, LWA_ALPHA);
}