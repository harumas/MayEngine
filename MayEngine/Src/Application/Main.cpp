#include "Win32Application.h"
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	auto result = CoInitializeEx(NULL, COINITBASE_MULTITHREADED);
	if (FAILED(result)) return 1;

	Application dxApp(1280, 720, L"DX Sample");
	Win32Application::Run(&dxApp, hInstance);
	return 0;
}
