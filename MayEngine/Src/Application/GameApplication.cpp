#include "GameApplication.h"

#include "AppInfo.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "Component/Material.h"
#include "Component/Renderer.h"
#include "Component/ShaderPass/HalfLambertPass.h"
#include "Component/ShaderPass/ShaderPassPool.h"

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
	Camera::current = camera;

	light = ObjectService::Create<Light>();

	auto halfLambertPass = ShaderPassPool::GetShaderPass<HalfLambertPass>();

	//FoV: 60
	//nearPlane: 1
	//farPlane: 1000
	camera->Init(XM_PIDIV2 * (2.0 / 3.0), 1.0f, 1000.0f);
	camera->transform->position = Vector3{ 0.0f, 0.0f, -10.0f };
	camera->SetYaw(0.0f);

	//草ブロックの作成
	grassBlock = GameObject::Create("grassBlock");
	auto renderer1 = grassBlock->AddComponent<Renderer>();
	auto material1 = ObjectService::Create<Material>();

	//メッシュとテクスチャのロード
	renderer1->LoadMesh("Assets/blocks/grass_block.fbx");
	material1->LoadTexture(L"Assets/textures/grass_block.png");
	material1->ApplyShaderPass(halfLambertPass);

	//描画コンポーネントにマテリアルを割り当てる
	renderer1->ApplyMaterial(material1);

	//人間の作成
	steve = GameObject::Create("steve");
	auto renderer2 = steve->AddComponent<Renderer>();
	auto material2 = ObjectService::Create<Material>();

	renderer2->LoadMesh("Assets/blocks/steve.fbx");
	material2->LoadTexture(L"Assets/textures/steve.png");
	material2->ApplyShaderPass(halfLambertPass);

	//位置の調整
	auto transform = steve->GetComponent<Transform>();
	transform->position.x += 4.0;
	transform->scale = transform->scale * 0.15;
	transform->rotation.y += XM_PI;
	transform->rotation.x += -XM_PIDIV2;

	//描画コンポーネントにマテリアルを割り当てる
	renderer2->ApplyMaterial(material2);

	renderPipeline->OnPostInit();
}

void GameApplication::Update()
{
	auto transform = grassBlock->GetComponent<Transform>();
	transform->rotation.y += 0.01;
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
