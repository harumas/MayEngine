#pragma once
#include "DXTexFix.h"
#include "RenderPipeline.h"
#include "Component/GameObject.h"
#include "Component/Light.h"
#include "Component/PointLight.h"

class GameApplication
{
public:
	GameApplication(unsigned int width, unsigned int height, const std::wstring& title);

	void Init(HINSTANCE hInstance, HWND hwnd);
	void Update();
	void Render();
	void Destroy();
private:
	shared_ptr<GameObject> teapot;
	shared_ptr<GameObject> steve;
	shared_ptr<GameObject> floor;
	shared_ptr<GameObject> cameraObject;
	shared_ptr<Light> light;
	shared_ptr<PointLight> pointLight;

	shared_ptr<RenderPipeline> renderPipeline;
};
