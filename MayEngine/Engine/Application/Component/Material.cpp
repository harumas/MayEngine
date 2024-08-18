#include "Material.h"

#include "../Utility/TextureLoader.h"


void Material::LoadTexture(const std::wstring& texturePath)
{
	TextureLoader::LoadTexture(mainTexture, texturePath);

	/*	auto uploadResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mainTexture.textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		pipeline->commandList->ResourceBarrier(1, &uploadResourceBarrier);*/
}

void Material::ApplyShaderPass(const std::shared_ptr<ShaderPass>& shaderPass)
{
	this->shaderPass = shaderPass;
}

void Material::SetPass()
{
	shaderPass->SetBuffer();

	const RenderPipeline& pipeline = RenderPipeline::GetInstance();
	const ComPtr<ID3D12PipelineState>& state = shaderPass->pipelineState;

	pipeline.commandList->SetPipelineState(state.Get());

	if (mainTexture.bufferId != UINT16_ERROR)
	{
		auto handle = pipeline.GetGPUDescriptorHandle(mainTexture.bufferId);
		pipeline.commandList->SetGraphicsRootDescriptorTable(4, handle);
	}
}

