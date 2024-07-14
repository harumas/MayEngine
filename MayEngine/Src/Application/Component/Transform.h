#pragma once
#include "Component.h" 
#include "../Utility/MathHelper.h"

using namespace DirectX;

class Transform : public Component
{
public:
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	Transform(const std::shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr),
		position(0, 0, 0),
		rotation(0, 0, 0),
		scale(1, 1, 1)
	{
	}

	XMMATRIX GetMatrix() const
	{
		return XMMatrixScalingFromVector(scale)
			* XMMatrixRotationRollPitchYawFromVector(rotation)
			* XMMatrixTranslationFromVector(position);
	}
};

