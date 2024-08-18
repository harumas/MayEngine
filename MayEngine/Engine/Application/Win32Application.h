#pragma once
#include <functional>
#include <windows.h>
#include <WinUser.h>

class Win32Application
{
public:
	using InitializeCall = std::function<void(HWND hwnd, HINSTANCE hInstance)>;
	using UpdateCall = std::function<void()>;
	using DisposeCall = std::function<void()>;

	Win32Application(HINSTANCE hInstance, const InitializeCall& initializeCall, const UpdateCall& updateCall, const DisposeCall& disposeCall);
	void InitWindow();
	void Run();

private:
	HINSTANCE hInstance;
	WNDCLASSEX windowClass = {};
	InitializeCall initializeCall;
	UpdateCall updateCall;
	DisposeCall disposeCall;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};