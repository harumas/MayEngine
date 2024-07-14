#pragma once
#include "DXTexFix.h"
#include "RenderPipeline.h"
#include "Component/GameObject.h"

class GameApplication
{
public:
	GameApplication(unsigned int width, unsigned int height, const std::wstring& title);

	void Init(HWND hwnd);
	void Update();
	void Render();
	void Destroy();
private:
	shared_ptr<GameObject>	 testObject;
	shared_ptr<GameObject> cameraObject;

	shared_ptr<RenderPipeline> renderPipeline;
};
