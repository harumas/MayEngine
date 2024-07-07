#pragma once
#include "../Utility/Transform.h"

class Camera
{
public:
	Camera();
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix(float aspectRatio) const;

	void Init(float fov, float nearPlane, float farPlane);
	void SetYaw(float yaw);
	void SetPitch(float pitch);

	Transform transform;

private:
	float fov;
	float nearPlane;
	float farPlane;
};

