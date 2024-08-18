#include "Win32Application.h"
#include <tchar.h>
#include "AppInfo.h"

Win32Application::Win32Application(HINSTANCE hInstance, const InitializeCall& initializeCall, const UpdateCall& updateCall, const DisposeCall& disposeCall) :
	hInstance(hInstance),
	initializeCall(initializeCall),
	updateCall(updateCall),
	disposeCall(disposeCall)
{

}

void Win32Application::InitWindow()
{
	// ウィンドウクラス生成
	windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = _T("DXSampleClass");
	RegisterClassEx(&windowClass);

	// ウィンドウサイズの調整
	RECT windowRect = { 0, 0, AppInfo::GetWindowWidth(), AppInfo::GetWindowHeight() };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(
		windowClass.lpszClassName,
		AppInfo::GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	initializeCall(hwnd, hInstance);

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

}

void Win32Application::Run()
{
	// メインループ
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// アプリケーション更新
			updateCall();
		}
	}

	// アプリケーション終了
	disposeCall();

	// クラスを登録解除する
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

LRESULT CALLBACK Win32Application::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return 0;
}