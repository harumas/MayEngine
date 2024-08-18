#pragma once
#include "../ShaderPass.h"

class HalfLambertPass : public ShaderPass
{
public:
	HalfLambertPass()
	{
		shaderPath = L"Engine/Shaders/HalfLambertShaders.hlsl";

		psoDesc = {};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);         // ���X�^���C�U�[�X�e�[�g
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);                   // �u�����h�X�e�[�g
		psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;                           // �T���v���}�X�N�̐ݒ�
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;   // �g�|���W�^�C�v
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;    // �X�g���b�v���̃J�b�g�ݒ�
		psoDesc.NumRenderTargets = 1;                                             // �����_�[�^�[�Q�b�g��
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;                       // �����_�[�^�[�Q�b�g�t�H�[�}�b�g
		psoDesc.SampleDesc.Count = 1;                                             // �}���`�T���v�����O�̐ݒ�

		// �[�x�X�e���V�� 
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;                                // �[�x�o�b�t�@�[�Ŏg�p����t�H�[�}�b�g
		psoDesc.DepthStencilState.DepthEnable = true;                             // �[�x�o�b�t�@�[���g�p���邩
		psoDesc.DepthStencilState.StencilEnable = false;                          // �X�e���V���e�X�g���s����
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;         // ��������
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;    // �����������̗p����
	}
};
