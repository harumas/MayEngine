#pragma once
#include "Component.h"
#include "../RenderPipeline.h"

class Light : Object
{
public:
	// ライティング用
	DirectX::XMVECTOR ambientLight = { 0.35f, 0.35f, 0.35f };
	DirectX::XMVECTOR lightColor = { 0.8f, 0.8f, 1.0f };
	DirectX::XMVECTOR lightDirection = { 0.3f, 0.3f, 0.8f };

	void CreateLightingBuffer();
	void CreateLightingBufferView(D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle);
private:
	struct LightingData
	{
		DirectX::XMVECTOR ambientLight;
		DirectX::XMVECTOR lightColor;
		DirectX::XMVECTOR lightDirection;
	};

	ComPtr<ID3D12Resource> constLightingBuffer;

	LightingData* mapLightingData;
};
