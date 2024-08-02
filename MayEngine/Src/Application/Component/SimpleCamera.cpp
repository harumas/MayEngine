#include "SimpleCamera.h"

#include "../Input/Input.h"

void SimpleCamera::OnUpdate()
{
	auto transform = Camera::current->transform;

	if (Input::IsKeyPressed(Key::W))
	{
		transform->position.z += 0.1f;
	}
	if (Input::IsKeyPressed(Key::S))
	{
		transform->position.z -= 0.1f;
	}
	if (Input::IsKeyPressed(Key::D))
	{
		transform->position.x += 0.1f;
	}
	if (Input::IsKeyPressed(Key::A))
	{
		transform->position.x -= 0.1f;
	}



}
