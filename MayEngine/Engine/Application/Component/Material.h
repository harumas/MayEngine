#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "ObjectService.h"
#include "ShaderPass.h"
#include "../Utility/Texture.h"
#include "../RenderPipeline.h"

class Material : public Object
{
public:
	Texture mainTexture;
	std::shared_ptr<ShaderPass> shaderPass;

	void LoadTexture(const std::wstring& texturePath);
	void ApplyShaderPass(const std::shared_ptr<ShaderPass>& shaderPass);
	void SetPass();
};
