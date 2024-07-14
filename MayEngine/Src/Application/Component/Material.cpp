#include "Material.h"


void Material::CreateShaderResourceBuffer(const wstring& texturePath)
{
	if (auto pipeline = RenderPipeline::instance)
	{
		DirectX::TexMetadata metadata = {};
		DirectX::ScratchImage scratchImg = {};

		std::vector<D3D12_SUBRESOURCE_DATA> textureSubresources;
		{
			ThrowIfFailed(LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratchImg));
			ThrowIfFailed(DxTexFix::PrepareUpload(pipeline->device_.Get(), scratchImg.GetImages(), scratchImg.GetImageCount(), metadata, textureSubresources));
		}

		// シェーダーリソースビュー(SRV)の生成
			// テクスチャバッファの生成
		textureDesc = {};
		textureDesc.Format = metadata.format;
		textureDesc.Width = static_cast<UINT>(metadata.width);
		textureDesc.Height = static_cast<UINT>(metadata.height);
		textureDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
		textureDesc.MipLevels = metadata.mipLevels;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		auto textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(pipeline->device_->CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(textureBuffer.ReleaseAndGetAddressOf())));

		ComPtr<ID3D12Resource> textureUploadBuffer;
		{
			const UINT64 textureBufferSize = GetRequiredIntermediateSize(textureBuffer.Get(), 0, 1);
			auto textureUploadHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto textureUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(textureBufferSize);
			ThrowIfFailed(pipeline->device_->CreateCommittedResource(
				&textureUploadHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&textureUploadDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&textureUploadBuffer)));
		}

		// テクスチャバッファの転送
		UpdateSubresources(pipeline->commandList_.Get(), textureBuffer.Get(), textureUploadBuffer.Get(), 0, 0, textureSubresources.size(), textureSubresources.data());
		auto uploadResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		pipeline->commandList_->ResourceBarrier(1, &uploadResourceBarrier);
	}
}

void Material::CreateShaderResourceView(UINT heapIndex)
{
	if (auto pipeline = RenderPipeline::instance)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = pipeline->GetDescriptorHandle(heapIndex);

		// シェーダーリソースビューの生成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		pipeline->device_->CreateShaderResourceView(textureBuffer.Get(), &srvDesc, descriptorHandle);
	}
}

void Material::OnUpdate()
{
}
