#include "Light.h"

void Light::CreateLightingBuffer()
{
	if (auto pipeline = RenderPipeline::instance)
	{
		const auto constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto constDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(mapLightingData) + 0xff) & ~0xff);

		ThrowIfFailed(pipeline->device_->CreateCommittedResource(
			&constHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&constDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constLightingBuffer.ReleaseAndGetAddressOf())));
	}
}

void Light::CreateLightingBufferView(D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle)
{
	if (auto pipeline = RenderPipeline::instance)
	{
		// 定数バッファービューの生成
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = constLightingBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = static_cast<UINT>(constLightingBuffer->GetDesc().Width);
		pipeline->device_->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

		// 定数情報のコピー
		constLightingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapLightingData));
		mapLightingData->ambientLight = ambientLight;
		mapLightingData->lightColor = lightColor;
		mapLightingData->lightDirection = lightDirection;
		constLightingBuffer->Unmap(0, nullptr);
	}
}
