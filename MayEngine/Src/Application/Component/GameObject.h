#pragma once
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "Component.h"

class Transform;
using namespace std;

class GameObject
{
public:
	explicit GameObject(const string& name) : name(name)
	{
		AddComponent<Transform>();
	}

	string name;

	template<typename T, typename... Ts>
	shared_ptr<T> AddComponent(Ts&&... params)
	{
		shared_ptr<Component> component = make_shared<T>(params);
		components.emplace(typeid(T), component);

		component->OnCreate();

		return component;
	}

	template<typename T>
	shared_ptr<T> GetComponent()
	{
		return components[typeid(T)];
	}

private:
	unordered_map<type_index, shared_ptr<Component>> components;
};
