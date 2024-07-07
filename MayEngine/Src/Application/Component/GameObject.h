#pragma once
#include "../RenderPipeline.h"

class GameObject
{
public:
	GameObject() = default;

	template<typename T, typename... Ts>
	T AddComponent();
private:
	vector<shared_ptr<Component>> components;
};
