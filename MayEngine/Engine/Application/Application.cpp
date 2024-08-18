#include "Application.h"

#include "AppInfo.h"
#include "Input/Input.h"
#include <windows.h>
#include <string>
#include "RenderPipeline.h"
#include "../Scene/SceneManager.h"

Application::Application(HINSTANCE hInstance, unsigned int width, unsigned int height, const std::wstring& title) :
	winApplication(hInstance, [this](HWND hwnd, HINSTANCE hInstance) { SetUp(hwnd, hInstance); }, [this]() { Update(); }, [this]() { Dispose(); })
{
	AppInfo::SetWindowTitle(title);
	AppInfo::SetWindowSize(width, height);
}

void Application::Run(int bootSceneIndex)
{
	ThrowIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED));

	winApplication.InitWindow();

	sceneManager.Switch(bootSceneIndex);

	RenderPipeline& renderPipeline = RenderPipeline::GetInstance();
	renderPipeline.OnPostInit();

	winApplication.Run();
}

void Application::SetUp(HWND hwnd, HINSTANCE hInstance)
{
	Input::Initialize();

	RenderPipeline& renderPipeline = RenderPipeline::Create();
	renderPipeline.OnInit(hwnd);
}

void Application::Update()
{
	const std::unique_ptr<BaseScene>& scene = sceneManager.GetCurrentScene();
	RenderPipeline& renderPipeline = RenderPipeline::GetInstance();

	Input::Update();

	scene->Update();

	renderPipeline.OnUpdate();

	Input::PostUpdate();

	renderPipeline.OnRender();
}

void Application::Dispose()
{
	Input::Shutdown();

	RenderPipeline& renderPipeline = RenderPipeline::GetInstance();
	renderPipeline.OnDestroy();
}
