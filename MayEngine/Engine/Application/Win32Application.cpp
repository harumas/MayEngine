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
	// �E�B���h�E�N���X����
	windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = _T("DXSampleClass");
	RegisterClassEx(&windowClass);

	// �E�B���h�E�T�C�Y�̒���
	RECT windowRect = { 0, 0, AppInfo::GetWindowWidth(), AppInfo::GetWindowHeight() };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̐���
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

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

}

void Win32Application::Run()
{
	// ���C�����[�v
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
			// �A�v���P�[�V�����X�V
			updateCall();
		}
	}

	// �A�v���P�[�V�����I��
	disposeCall();

	// �N���X��o�^��������
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