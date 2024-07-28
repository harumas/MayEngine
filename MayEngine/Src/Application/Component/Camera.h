#pragma once
#include "Component.h"
#include "GameObject.h"
#include "../Component/Transform.h"

class Camera : public Component
{
public:
	Camera(const shared_ptr<GameObject>& gameObject);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix(float aspectRatio) const;

	void Init(float fov, float nearPlane, float farPlane);
	void SetYaw(float yaw);
	void SetPitch(float pitch);

	shared_ptr<Transform> transform;
	static shared_ptr<Camera> current;
private:
	float fov;
	float nearPlane;
	float farPlane;
};

