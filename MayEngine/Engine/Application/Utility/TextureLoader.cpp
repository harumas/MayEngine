#include "TextureLoader.h"

#include <DirectXTex.h>
#include "../Helper.h"
#include "../RenderPipeline.h"

UINT TextureLoader::idCounter = 0;

void TextureLoader::LoadTexture(Texture& texture, const std::wstring& texturePath)
{
	const RenderPipeline& pipeline = RenderPipeline::GetInstance();

	DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImg = {};

	std::vector<D3D12_SUBRESOURCE_DATA> textureSubresources;
	{
		ThrowIfFailed(LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratchImg));
	}

	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//�o�b�t�@�̍쐬
	auto textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	ThrowIfFailed(pipeline.device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(texture.textureBuffer.ReleaseAndGetAddressOf())));

	//���f�[�^���o
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	ThrowIfFailed(texture.textureBuffer->WriteToSubresource(
		0,
		nullptr,	//�S�̈�R�s�[
		img->pixels,	//���f�[�^�A�h���X
		(UINT)img->rowPitch,	//1���C���T�C�Y
		(UINT)img->slicePitch	//1���T�C�Y
	));

	// �e�N�X�`���o�b�t�@�̓]��
	texture.bufferId = idCounter++;
	D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = pipeline.GetCPUDescriptorHandle(texture.bufferId);

	// �V�F�[�_�[���\�[�X�r���[�̐���
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = metadata.format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	pipeline.device->CreateShaderResourceView(texture.textureBuffer.Get(), &srvDesc, descriptorHandle);
}

