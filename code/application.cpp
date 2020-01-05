#include"stdafx.h"
#include"application.h"

#pragma comment(lib,"D3D11.lib")

namespace
{	
	ID3D11Device*            d3dDevice = nullptr;
	ID3D11DeviceContext*     d3dDeviceContext = nullptr;
	IDXGISwapChain*          swapChain = nullptr;
	ID3D11RenderTargetView*  mainRenderTargetView = nullptr;

	void CleanupDeviceD3D()
	{
		safe_release(mainRenderTargetView);
		safe_release(swapChain);
		safe_release(d3dDeviceContext);
		safe_release(d3dDevice);
	}

	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
		safe_release(pBackBuffer);
	}

	bool CreateDeviceD3D(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

		HRESULT result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevelArray,
			2,
			D3D11_SDK_VERSION,
			&sd,
			&swapChain,
			&d3dDevice,
			&featureLevel,
			&d3dDeviceContext);

		if(result != S_OK)
		{
			return false;
		}

		CreateRenderTarget();
		return true;
	}
}


Application::Application()
{
	std::setlocale(LC_ALL, "ja_JP.UTF-8");

	Window::Desc desc;
	desc.caption = L"sand box";
	desc.hInstance = ::GetModuleHandle(nullptr);
	desc.clientData = this;

	mWindow.create(desc);
	mWindow.OnMessage += updateMessage;

	// Initialize Direct3D
	if(CreateDeviceD3D(mWindow.getHandle())==false)
	{
		mKilled = true;
		return;
	}

	mWindow.show();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(mWindow.getHandle());
	ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);

	mBackgroundColor = ImVec4(0.5f, 0.5f, 0.5f, 1);
}


Application::Application(Window::Desc& desc)
{
	desc.clientData = this;

	mWindow.create(desc);
	mWindow.OnMessage += updateMessage;

	mWindow.show();
}


Application::~Application()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	mWindow.OnMessage -= updateMessage;

	CleanupDeviceD3D();
	ImGui::DestroyContext();
}


void Application::update(Action action)
{
	while(mKilled == false)
	{
		mWindow.updateMessage();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if(mShowImGuiDemoWindow)
		{
			ImGui::ShowDemoWindow(&mShowImGuiDemoWindow);
		}

		action();

		// Rendering
		ImGui::Render();
		d3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		d3dDeviceContext->ClearRenderTargetView(mainRenderTargetView, (float*)&mBackgroundColor);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swapChain->Present(1, 0); // Present with vsync
	}
}


// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Application::updateMessage(Window::MessageInfo* info)
{
	ImGui_ImplWin32_WndProcHandler(info->hWnd, info->msg, info->wParam, info->lParam);

	auto app = static_cast<Application*>(info->clientData);

	switch(info->msg)
	{
	case WM_SIZE:
		if(d3dDevice != nullptr && info->wParam != SIZE_MINIMIZED)
		{
			safe_release(mainRenderTargetView);
			swapChain->ResizeBuffers(0, (UINT)LOWORD(info->lParam), (UINT)HIWORD(info->lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
			info->handled = true;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		app->mKilled = true;
		break;
	case WM_ACTIVATE:
		switch(LOWORD(info->wParam))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			break;
		}
		break;
	}
}