#include "ShaderPassPool.h"


std::unordered_map<std::type_index, std::shared_ptr<ShaderPass>> ShaderPassPool::shaderPassPool;

