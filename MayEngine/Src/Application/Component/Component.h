#pragma once
#include "GameObject.h"
#include "ObjectService.h"

class Component : public Object
{
public:
	weak_ptr<GameObject> gameObject;

	explicit Component(const shared_ptr<GameObject>& gameObjectPtr);
	virtual ~Component() {}
};
