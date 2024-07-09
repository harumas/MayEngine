#include "Camera.h"

Camera::Camera(const shared_ptr<GameObject>& gameObject) : Component(gameObject),
                                                           fov(0),
                                                           nearPlane(0),
                                                           farPlane(0)
{
	transform = gameObject->GetComponent<Transform>();
	transform->rotation = Vector3(0, XM_PI, 0);
}

void Camera::Init(float fov, float nearPlane, float farPlane)
{
	this->fov = fov;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
}

XMMATRIX Camera::GetViewMatrix() const
{
	const XMVECTOR& rotation = XMQuaternionRotationRollPitchYawFromVector(transform->rotation);
	const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
	const XMVECTOR eyeDirection = XMVector3TransformNormal(Vector3::FORWARD, rotationMatrix);

	return XMMatrixLookToLH(transform->position, eyeDirection, Vector3::UP);
}

XMMATRIX Camera::GetProjectionMatrix(float aspectRatio) const
{
	return XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::SetYaw(float yaw)
{
	transform->rotation.y = yaw;
}

void Camera::SetPitch(float pitch)
{
	transform->rotation.x = pitch;
}
