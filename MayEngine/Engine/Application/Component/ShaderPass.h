#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include "../d3dx12.h"
#include "../RenderPipeline.h"
#include <dxgiformat.h>
#include <windows.h>
#include <combaseapi.h>
#include <d3dcommon.h>
#include <string>
#include "../Helper.h"

using Microsoft::WRL::ComPtr;

class ShaderPass
{
public:
	std::wstring shaderPath;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ComPtr<ID3D12PipelineState> pipelineState;         // �p�C�v���C���X�e�[�g

	void Compile()
	{
		const RenderPipeline& pipeline = RenderPipeline::GetInstance();

		// �p�C�v���C���X�e�[�g�̐���
		// �V�F�[�_�[�I�u�W�F�N�g�̐���
#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		ComPtr<ID3DBlob> vsBlob;
		ComPtr<ID3DBlob> psBlob;

		D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compileFlags, 0, &vsBlob, nullptr);
		D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compileFlags, 0, &psBlob, nullptr);

		// ���_���C�A�E�g�̐���
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g(PSO)�𐶐�
		psoDesc.pRootSignature = pipeline.rootSignature.Get();
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) }; // ���̓��C�A�E�g�̐ݒ�
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());                       // ���_�V�F�[�_
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());                       // �s�N�Z���V�F�[�_

		ThrowIfFailed(pipeline.device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf())));
	}

	virtual	void SetBuffer() {}
};
