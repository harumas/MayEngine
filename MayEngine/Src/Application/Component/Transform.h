#pragma once
#include "../Utility/MathHelper.h"
#include "Component.h"

using namespace DirectX;

class Transform : Component
{
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	Transform(const shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr),
		position(0, 0, 0),
		rotation(0, 0, 0),
		scale(1, 1, 1)
	{
	}

	void OnUpdate() override;

	XMMATRIX GetMatrix() const
	{
		return XMMatrixScalingFromVector(scale)
			* XMMatrixRotationRollPitchYawFromVector(rotation)
			* XMMatrixTranslationFromVector(position);
	}
};

inline void Transform::OnUpdate()
{
	if (auto pipeline = GetPipeline().lock())
	{

	}
}
