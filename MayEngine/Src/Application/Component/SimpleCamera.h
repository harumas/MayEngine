#pragma once
#include "Camera.h"

class SimpleCamera : public Camera
{
public:
	float moveSpeed = 0.1f;
	float rotateSpeed = 0.01f;

	explicit SimpleCamera(const shared_ptr<GameObject>& gameObject)
		: Camera(gameObject)
	{
	}

	void OnUpdate() override;
};
