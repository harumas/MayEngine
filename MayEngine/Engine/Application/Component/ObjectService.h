#pragma once
#include <memory>
#include <vector>

class Object
{
protected:
	Object()
	{

	}
	virtual ~Object() = default;

public:
	virtual void OnCreate() {}
	virtual void OnUpdate() {}
	virtual void OnDraw() {}
	virtual void OnDestroy() {}

private:
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;

	Object(const Object&&) = delete;
	Object& operator=(const Object&&) = delete;
};


class ObjectService
{
	static std::vector<std::shared_ptr<Object>> objects;

public:
	template<typename T, typename... Ts>
	static std::shared_ptr<T> Create(Ts&&... params)
	{
		std::shared_ptr<T> ptr = std::make_shared<T>(params...);

		objects.push_back(ptr);

		ptr->OnCreate();
		return ptr;
	}

	static void Register(const std::shared_ptr<Object>& object)
	{
		objects.push_back(object);
	}

	static void ProcessOnUpdate()
	{
		for (const auto& object : objects)
		{
			object->OnUpdate();
		}
	}

	static void ProcessOnDraw()
	{
		for (const auto& object : objects)
		{
			object->OnDraw();
		}
	}
};
