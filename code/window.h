#pragma once

#include<tchar.h>
#include<Windows.h>
#include"typeAlias.h"

class Window sealed
{
public:
	struct MessageInfo
	{
		HWND hWnd;
		UINT msg;
		WPARAM wParam;
		LPARAM lParam;
		void* clientData;
		bool handled;

		MessageInfo(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam, void* _clientData, bool _handled)
			: hWnd(_hWnd), msg(_msg), wParam(_wParam), lParam(_lParam), clientData(_clientData), handled(_handled) {}
	};

	struct Desc
	{
		u32			style;
		u32			width;
		u32			height;
		POINT		showPos;
		HWND		hWndParent;
		HINSTANCE	hInstance;
		void*		clientData;
		std::wstring caption;

		union
		{
			u32 flags;
			struct
			{
				u32 fullScreenEnable : 1;
				u32 autoCalcShowPos : 1;
				u32 : 30;
			};
		};
		Desc() : style(WS_OVERLAPPEDWINDOW), width(640), height(480), flags(0x02), hWndParent(nullptr), hInstance(nullptr), clientData(nullptr) {}
	};

public:
	Window();
	~Window();

	const std::wstring& getCaption() const
	{
		return mInfo.caption;
	}

	bool setCaption(const std::wstring& caption)
	{
		mInfo.caption.assign(caption);
		return !!::SetWindowText(mInfo.hWnd, caption.c_str());
	}

	void setAlpha(u8 alpha)
	{
		SetLayeredWindowAttributes(mInfo.hWnd, 50, alpha, LWA_ALPHA);
	}

	void setParent(Window* parent)
	{
		SetParent(mInfo.hWnd, parent->mInfo.hWnd);
	}

	const HWND& getHandle() const
	{
		return mInfo.hWnd;
	}

	void setWindowSize(u32 width, u32 height)
	{
		updateRect(width, height, mInfo.style);
	}

	u32	getWidth() const
	{
		return mInfo.width;
	}

	void setWidth(u32 width)
	{
		updateRect(width, mInfo.height, mInfo.style);
	}

	u32	getHeight() const
	{
		return mInfo.height;
	}

	void setHeight(u32 height)
	{
		updateRect(mInfo.width, height, mInfo.style);
	}

	u32	getClientWidth() const
	{
		return mInfo.clientWidth;
	}

	u32	getClientHeight() const
	{
		return mInfo.clientHeight;
	}

	bool isActive()
	{
		return (mInfo.hWnd == ::GetForegroundWindow());
	}

	bool isDead() const
	{
		return mInfo.isDead;
	}

	bool isWindowed() const
	{
		return mInfo.isWindowed != 0;
	}

	void show()
	{
		::ShowWindow(getHandle(), SW_SHOWNORMAL);
	}

	void create(const Desc& desc);
	void updateMessage();

	static LRESULT	CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Delegate<Window::MessageInfo*> OnMessage;

private:
	struct Info
	{
		HWND		hWnd;
		HWND		hWndParent;
		void*		clientData;
		u32			width;
		u32			height;
		u32			clientWidth;
		u32			clientHeight;
		DWORD		style;
		MSG			msg;
		WNDCLASSEX	winEx;
		std::wstring caption;
		std::wstring regName;

		union
		{
			u32 attribute;
			struct
			{
				u32		isDead : 1;
				u32		isParent : 1;
				u32		isWindowed : 1;
				u32		reseve : 29;
			};
		};

		Info() : hWnd(nullptr), hWndParent(nullptr), clientData(nullptr), attribute(0) {}
	};

private:
	Info mInfo;

private:
	void updateRect(u32 width, u32 height, DWORD style);
	void registerInstance(const HINSTANCE hInst);
	void createInternal(const Desc& desc);
};
