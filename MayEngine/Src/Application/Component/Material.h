#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "ObjectService.h"
#include "../RenderPipeline.h"

class Material : Object
{
public:
	void CreateShaderResourceBuffer();
	void CreateShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle);
private:
	D3D12_RESOURCE_DESC textureDesc = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureBuffer_;
};
