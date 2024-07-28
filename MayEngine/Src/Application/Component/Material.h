#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "ObjectService.h"
#include "../Utility/Texture.h"
#include "../RenderPipeline.h"

class Material : public Object
{
public:
	Texture mainTexture;

	void LoadTexture(const wstring& texturePath);
	void SetPass();
};
