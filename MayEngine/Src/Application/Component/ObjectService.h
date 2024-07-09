#pragma once
#include <memory>
#include <vector>

using namespace std;

class Object
{
protected:
	Object()
	{

	}
	virtual ~Object() = default;

public:
	virtual void OnCreate();
	virtual void OnUpdate();
	virtual void OnDraw();
	virtual void OnDestroy();

private:
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;

	Object(const Object&&) = delete;
	Object& operator=(const Object&&) = delete;
};


class ObjectService
{
	static vector<shared_ptr<Object>> objects;

public:
	template<typename T, typename... Ts>
	static std::shared_ptr<T> Create(Ts&&... params)
	{
		shared_ptr<T> ptr = std::make_shared<T>(params...);

		objects.push_back(ptr);

		ptr->OnCreate();
		return ptr;
	}

	static void SendOnUpdate()
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
