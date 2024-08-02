#pragma once
#include "ObjectService.h"

class GameObject;

class Component : public Object
{
public:
	weak_ptr<GameObject> gameObject;

	explicit Component(const shared_ptr<GameObject>& gameObjectPtr);
	virtual ~Component() {}
};
