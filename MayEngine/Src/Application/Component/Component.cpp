#include "Component.h"

Component::Component(const shared_ptr<GameObject>& gameObjectPtr) :
	gameObject(gameObjectPtr)
{
}
