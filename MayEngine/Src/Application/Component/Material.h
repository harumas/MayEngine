#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "ObjectService.h"
#include "../RenderPipeline.h"

class Material : public Object
{
public:
	void CreateShaderResourceBuffer(const wstring& texturePath);
	void CreateShaderResourceView(UINT heapIndex);
	void OnUpdate() override;
private:
	D3D12_RESOURCE_DESC textureDesc = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureBuffer;
};
