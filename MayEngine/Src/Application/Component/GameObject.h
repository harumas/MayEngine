#pragma once
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "Component.h"
#include "Transform.h"
#include "ObjectService.h"

using namespace std;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	static shared_ptr<GameObject> Create(const string& name)
	{
		shared_ptr<GameObject> object = make_shared<GameObject>(name);
		object->AddComponent<Transform>();
		return object;
	}

	explicit GameObject(const string& name) : name(name)
	{}

	string name;

	template<typename T>
	shared_ptr<T> AddComponent()
	{
		shared_ptr<T> component = make_shared<T>(shared_from_this());
		components.emplace(typeid(T), component);
		ObjectService::Register(dynamic_pointer_cast<T>(component));

		component->OnCreate();

		return component;
	}

	template<typename T>
	shared_ptr<T> GetComponent()
	{
		return dynamic_pointer_cast<T>(components[typeid(T)]);
	}

private:


	unordered_map<type_index, shared_ptr<Component>> components;
};
