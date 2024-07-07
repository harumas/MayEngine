#pragma once
#include "../Utility/ObjectService.h"

class Material : Object
{
public:
	void CreateShaderResourceBuffer();
	void CreateShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle);
private:
	D3D12_RESOURCE_DESC textureDesc = {};
	ComPtr<ID3D12Resource> textureBuffer_;
};
