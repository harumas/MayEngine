#include "Material.h"

#include "../Utility/TextureLoader.h"


void Material::LoadTexture(const wstring& texturePath)
{
	if (auto pipeline = RenderPipeline::instance)
	{
		TextureLoader::LoadTexture(mainTexture, texturePath);

		/*	auto uploadResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mainTexture.textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			pipeline->commandList_->ResourceBarrier(1, &uploadResourceBarrier);*/
	}
}

void Material::SetPass()
{
	if (auto pipeline = RenderPipeline::instance)
	{
		auto handle = pipeline->GetGPUDescriptorHandle(mainTexture.bufferId);
		pipeline->commandList_->SetGraphicsRootDescriptorTable(2, handle);
	}
}

