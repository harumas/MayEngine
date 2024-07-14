#include "GameApplication.h"

#include "AppInfo.h"
#include "Component/Camera.h"
#include "Component/Material.h"
#include "Component/Renderer.h"

GameApplication::GameApplication(unsigned int width, unsigned int height, const std::wstring& title)
{
	AppInfo::SetWindowTitle(title);
	AppInfo::SetWindowSize(width, height);
}

void GameApplication::Init(HWND hwnd)
{
	renderPipeline = RenderPipeline::Create();
	renderPipeline->OnInit(hwnd);

	cameraObject = GameObject::Create("cameraObject");
	auto camera = cameraObject->AddComponent<Camera>();

	//FoV: 90
	//nearPlane: 1
	//farPlane: 1000
	camera->Init(XM_PIDIV2, 1.0f, 1000.0f);
	camera->transform->position = Vector3{ 0.0f, 3.0f, -10.0f };
	camera->SetYaw(0.0f);

	testObject = GameObject::Create("testObject");
	auto renderer = testObject->AddComponent<Renderer>();
	renderer->LoadMesh("Assets/blocks/grass_block.fbx");

	auto material = ObjectService::Create<Material>();
	material->CreateShaderResourceBuffer(L"Assets/textures/grass_block.png");
	material->CreateShaderResourceView(0);

	renderPipeline->OnPostInit();
}

void GameApplication::Update()
{
	renderPipeline->OnUpdate();
}

void GameApplication::Render()
{
	renderPipeline->OnRender();
}

void GameApplication::Destroy()
{
	renderPipeline->OnDestroy();
}
