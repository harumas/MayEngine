#pragma once
#include "Camera.h"

class SimpleCamera : public Camera
{
public:
	float moveSpeed = 1.0f;
	float rotateSpeed = 1.0f;

	explicit SimpleCamera(const shared_ptr<GameObject>& gameObject)
		: Camera(gameObject)
	{}

	void OnUpdate() override;
};
