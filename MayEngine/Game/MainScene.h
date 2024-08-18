#pragma once
#include <memory>

#include "../Engine/Scene/BaseScene.h"
#include "../Engine/Application/Component/GameObject.h"
#include "../Engine/Application/Component/Light.h"
#include "../Engine/Application/Component/PointLight.h"

class MainScene : public BaseScene
{
public:
	void Initialize() override;
	void Update() override;

	std::string_view GetSceneName() override
	{
		return "MainScene";
	}

private:
	std::shared_ptr<GameObject> teapot;
	std::shared_ptr<GameObject> steve;
	std::shared_ptr<GameObject> floor;
	std::shared_ptr<GameObject> pLight;
	std::shared_ptr<GameObject> cameraObject;
	std::shared_ptr<Light> light;
	std::shared_ptr<PointLight> pointLight;
};
