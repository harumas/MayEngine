#pragma once
#include <d3d12.h>
#include <intsafe.h>
#include <wrl/client.h>

#include "../d3dx12.h"
#include "../Helper.h"
#include <windows.h>
#include <combaseapi.h>

using Microsoft::WRL::ComPtr;

template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList);

	void SetBufferData(const T& buffer);
	void SetConstantBufferView(UINT rootParameter);

private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Resource> constantBuffer;
};

template <typename T>
ConstantBuffer<T>::ConstantBuffer(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	ConstantBuffer::device = device;
	ConstantBuffer::commandList = commandList;

	// �萔�o�b�t�@�[�̐���
	const auto constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	constexpr auto size = (sizeof(T) + 0xff) & ~0xff;
	const CD3DX12_RESOURCE_DESC constDesc = CD3DX12_RESOURCE_DESC::Buffer(size); // 256�A���C�����g�ŃT�C�Y���w��

	ThrowIfFailed(device->CreateCommittedResource(
		&constHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constantBuffer.ReleaseAndGetAddressOf())));
}

template <typename T>
void ConstantBuffer<T>::SetBufferData(const T& buffer)
{
	//�o�b�t�@�ɏ�������
	T* constMap = nullptr;
	const CD3DX12_RANGE readRange(0, 0);

	if (SUCCEEDED(constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&constMap))))
	{
		memcpy_s(constMap, sizeof(T), &buffer, sizeof(T));
		constantBuffer->Unmap(0, nullptr);
	}
}

template <typename T>
void ConstantBuffer<T>::SetConstantBufferView(UINT rootParameter)
{
	commandList->SetGraphicsRootConstantBufferView(rootParameter, constantBuffer->GetGPUVirtualAddress());
}


