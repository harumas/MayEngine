#include "Win32Application.h"
#include "RenderPipeline.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	auto result = CoInitializeEx(NULL, COINITBASE_MULTITHREADED);
	if (FAILED(result))
	{
		return 1;
	}

	GameApplication gameApp(1920, 1080, L"MayEngine");
	Win32Application::Run(gameApp, hInstance);
	return 0;
}
