#pragma once
#include <memory>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "../ShaderPass.h"

class ShaderPassPool
{
public:
	template<typename T>
	static std::shared_ptr<ShaderPass> GetShaderPass()
	{
		const std::type_index id = typeid(T);

		if (shaderPassPool.find(id) != shaderPassPool.end())
		{
			return shaderPassPool[id];
		}

		std::shared_ptr<T> shaderPass = std::make_shared<T>();
		shaderPass->Compile();

		shaderPassPool.emplace(id, shaderPass);
		return std::dynamic_pointer_cast<T>(shaderPass);
	}

private:
	static std::unordered_map<std::type_index, std::shared_ptr<ShaderPass>> shaderPassPool;
};
