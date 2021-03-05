#pragma once
#include <Windows.h>
#include "Graphics.h"
#include "Keyboard.h"

class Window
{
public:
	Window(int width, int height);
	~Window();
	HWND hWnd;
	Graphics& Gfx();
private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName =L"Direct3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	std::unique_ptr<Graphics> pGfx;

public:
	Keyboard kbd;
	
};