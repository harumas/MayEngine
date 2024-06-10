#pragma once
#include <d3d12.h>
#include <DirectXTex.h>
#include <dxgiformat.h>
#include <intsafe.h>
#include <sal.h>

#include "d3dx12.h"

namespace DxTexFix
{
#define HRESULT_E_NOT_SUPPORTED static_cast<HRESULT>(0x80070032L)
#define XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT DXGI_FORMAT(118)
#define XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS DXGI_FORMAT(119)
#define XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT DXGI_FORMAT(120)

	template<typename T, typename PT> void AdjustPlaneResource(
		_In_ DXGI_FORMAT fmt,
		_In_ size_t height,
		_In_ size_t slicePlane,
		_Inout_ T& res) noexcept
	{
		switch (static_cast<int>(fmt))
		{
		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
		case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
		case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
		case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
			if (!slicePlane)
			{
				// Plane 0
				res.SlicePitch = res.RowPitch * static_cast<PT>(height);
			}
			else
			{
				// Plane 1
				res.pData = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(res.pData) + res.RowPitch * PT(height));
				res.SlicePitch = res.RowPitch * static_cast<PT>((height + 1) >> 1);
			}
			break;

		case DXGI_FORMAT_NV11:
			if (!slicePlane)
			{
				// Plane 0
				res.SlicePitch = res.RowPitch * static_cast<PT>(height);
			}
			else
			{
				// Plane 1
				res.pData = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(res.pData) + res.RowPitch * PT(height));
				res.RowPitch = (res.RowPitch >> 1);
				res.SlicePitch = res.RowPitch * static_cast<PT>(height);
			}
			break;

		default:
			break;
		}
	}


	_Use_decl_annotations_

		inline HRESULT PrepareUpload(
			ID3D12Device* pDevice,
			const DirectX::Image* srcImages,
			size_t nimages,
			const DirectX::TexMetadata& metadata,
			std::vector<D3D12_SUBRESOURCE_DATA>& subresources)
	{
		if (!pDevice || !srcImages || !nimages || !metadata.mipLevels || !metadata.arraySize)
			return E_INVALIDARG;

		const UINT numberOfPlanes = D3D12GetFormatPlaneCount(pDevice, metadata.format);
		if (!numberOfPlanes)
			return E_INVALIDARG;

		if ((numberOfPlanes > 1) && DirectX::IsDepthStencil(metadata.format))
		{
			// DirectX 12 uses planes for stencil, DirectX 11 does not
			return HRESULT_E_NOT_SUPPORTED;
		}

		size_t numberOfResources = (metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE3D)
			? 1u : metadata.arraySize;
		numberOfResources *= metadata.mipLevels;
		numberOfResources *= numberOfPlanes;

		if (numberOfResources > D3D12_REQ_SUBRESOURCES)
			return E_INVALIDARG;

		subresources.clear();
		subresources.reserve(numberOfResources);

		// Fill out subresource array
		if (metadata.IsVolumemap())
		{
			//--- Volume case -------------------------------------------------------------
			if (!metadata.depth)
				return E_INVALIDARG;

			if (metadata.depth > UINT16_MAX)
				return E_INVALIDARG;

			if (metadata.arraySize > 1)
				// Direct3D 12 doesn't support arrays of 3D textures
				return HRESULT_E_NOT_SUPPORTED;

			for (size_t plane = 0; plane < numberOfPlanes; ++plane)
			{
				size_t depth = metadata.depth;

				for (size_t level = 0; level < metadata.mipLevels; ++level)
				{
					const size_t index = metadata.ComputeIndex(level, 0, 0);
					if (index >= nimages)
						return E_FAIL;

					const DirectX::Image& img = srcImages[index];

					if (img.format != metadata.format)
						return E_FAIL;

					if (!img.pixels)
						return E_POINTER;

					// Verify pixels in image 1 .. (depth-1) are exactly image->slicePitch apart
					// For 3D textures, this relies on all slices of the same miplevel being continous in memory
					// (this is how ScratchImage lays them out), which is why we just give the 0th slice to Direct3D 11
					const uint8_t* pSlice = img.pixels + img.slicePitch;
					for (size_t slice = 1; slice < depth; ++slice)
					{
						const size_t tindex = metadata.ComputeIndex(level, 0, slice);
						if (tindex >= nimages)
							return E_FAIL;

						const DirectX::Image& timg = srcImages[tindex];

						if (!timg.pixels)
							return E_POINTER;

						if (timg.pixels != pSlice
							|| timg.format != metadata.format
							|| timg.rowPitch != img.rowPitch
							|| timg.slicePitch != img.slicePitch)
							return E_FAIL;

						pSlice = timg.pixels + img.slicePitch;
					}

					D3D12_SUBRESOURCE_DATA res =
					{
						img.pixels,
						static_cast<LONG_PTR>(img.rowPitch),
						static_cast<LONG_PTR>(img.slicePitch)
					};

					AdjustPlaneResource<D3D12_SUBRESOURCE_DATA, intptr_t>(metadata.format, img.height, plane, res);

					subresources.emplace_back(res);

					if (depth > 1)
						depth >>= 1;
				}
			}
		}
		else
		{
			//--- 1D or 2D texture case ---------------------------------------------------
			for (size_t plane = 0; plane < numberOfPlanes; ++plane)
			{
				for (size_t item = 0; item < metadata.arraySize; ++item)
				{
					for (size_t level = 0; level < metadata.mipLevels; ++level)
					{
						const size_t index = metadata.ComputeIndex(level, item, 0);
						if (index >= nimages)
							return E_FAIL;

						const DirectX::Image& img = srcImages[index];

						if (img.format != metadata.format)
							return E_FAIL;

						if (!img.pixels)
							return E_POINTER;

						D3D12_SUBRESOURCE_DATA res =
						{
							img.pixels,
							static_cast<LONG_PTR>(img.rowPitch),
							static_cast<LONG_PTR>(img.slicePitch)
						};

						AdjustPlaneResource<D3D12_SUBRESOURCE_DATA, intptr_t>(metadata.format, img.height, plane, res);

						subresources.emplace_back(res);
					}
				}
			}
		}

		return S_OK;
	}


}


