#include "MainScene.h"
#include "../Engine/Application/Component/SimpleCamera.h"
#include "../Engine/Application/Component/ShaderPass/ShaderPassPool.h"
#include "../Engine/Application/Component/ShaderPass/PhongPass.h"
#include "../Engine/Application/Component/ShaderPass/HalfLambertPass.h"
#include "../Engine/Application/Component/ShaderPass/StandardPass.h"
#include "../Engine/Application/Component/Renderer.h"

void MainScene::Initialize()
{
	cameraObject = GameObject::Create("cameraObject");
	auto camera = cameraObject->AddComponent<SimpleCamera>();
	Camera::current = camera;

	light = ObjectService::Create<Light>();
	pointLight = ObjectService::Create<PointLight>();

	//pointLight->position.y -= 2.0f;
	pointLight->lightRange = 10.0f;
	pointLight->lightColor = Vector3{ 15.0f,15.0f,15.0f };
	pointLight->position.y = 2.0f;
	pointLight->position.z = -4.0f;

	auto halfLambertPass = ShaderPassPool::GetShaderPass<HalfLambertPass>();
	auto phongPass = ShaderPassPool::GetShaderPass<PhongPass>();
	auto standardPass = ShaderPassPool::GetShaderPass<StandardPass>();

	//FoV: 60
	//nearPlane: 1
	//farPlane: 1000
	camera->Init(XM_PIDIV2 * (2.0 / 3.0), 1.0f, 1000.0f);
	camera->transform->position = Vector3{ 0.0f, 0.0f, -10.0f };
	camera->SetYaw(0.0f);

	//草ブロックの作成
	pLight = GameObject::Create("plight");
	auto lightRenderer = pLight->AddComponent<Renderer>();
	auto lightMaterial = ObjectService::Create<Material>();

	//メッシュとテクスチャのロード
	lightRenderer->LoadMesh("Assets/sphere.fbx");
	lightMaterial->ApplyShaderPass(standardPass);
	auto lightTransform = pLight->GetComponent<Transform>();
	lightTransform->position = pointLight->position;
	lightTransform->scale = lightTransform->scale.VScale(0.1);
	lightRenderer->ApplyMaterial(lightMaterial);

	//草ブロックの作成
	teapot = GameObject::Create("teapot");
	auto renderer1 = teapot->AddComponent<Renderer>();
	auto material1 = ObjectService::Create<Material>();

	//メッシュとテクスチャのロード
	renderer1->LoadMesh("Assets/teapot.fbx");
	material1->ApplyShaderPass(phongPass);
	auto transform = teapot->GetComponent<Transform>();
	transform->position.y -= 2.0;
	transform->scale = transform->scale.VScale(2.5);

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
	transform = steve->GetComponent<Transform>();
	transform->position.x += 4.0;
	transform->scale = transform->scale * 0.15;
	transform->rotation.y += XM_PI;
	transform->rotation.x += -XM_PIDIV2;

	//描画コンポーネントにマテリアルを割り当てる
	renderer2->ApplyMaterial(material2);

	floor = GameObject::Create("floor");
	auto renderer3 = floor->AddComponent<Renderer>();
	auto material3 = ObjectService::Create<Material>();

	renderer3->LoadMesh("Assets/floor.fbx");
	material3->LoadTexture(L"Assets/textures/floor.png");
	material3->ApplyShaderPass(halfLambertPass);

	transform = floor->GetComponent<Transform>();
	transform->position.y -= 3.0;
	transform->scale.x *= 20.0;
	transform->scale.z *= 20.0;

	//描画コンポーネントにマテリアルを割り当てる
	renderer3->ApplyMaterial(material3);
}

void MainScene::Update()
{
	auto transform = teapot->GetComponent<Transform>();
	transform->rotation.y -= XM_2PI / 360;
}

