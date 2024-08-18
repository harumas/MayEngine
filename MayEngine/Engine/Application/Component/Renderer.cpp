#include "Renderer.h"

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "../AppInfo.h"
#include <dxgiformat.h>
#include <windows.h>
#include <combaseapi.h>
#include <d3d12.h>
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <string>
#include <vector>
#include "Component.h"
#include "Material.h"
#include "../d3dx12.h"
#include "../FbxLoader.h"
#include "../Helper.h"
#include "../RenderPipeline.h"
#include "../Utility/Mesh.h"

Renderer::Renderer(const std::shared_ptr<GameObject>& gameObjectPtr) :
	Component(gameObjectPtr),
	matrixBuffer(RenderPipeline::GetInstance().device, RenderPipeline::GetInstance().commandList)
{

}

void Renderer::LoadMesh(const std::string& filePath)
{
	// fbx���f���̃��[�h
	if (!FbxLoader::Load(filePath, &mesh))
	{
		ThrowMessage("failed load fbx file.");
	}

	//�O�p�|���S��
	mesh.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	const RenderPipeline& pipeline = RenderPipeline::GetInstance();

	// ���_�o�b�t�@�r���[�̐���
	{
		// ���_���W
		std::vector<Vertex> vertices = mesh.vertices;
		const UINT vertexBufferSize = sizeof(Vertex) * vertices.size();
		auto vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto vertexResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		// ���_�o�b�t�@�[�̐���
		ThrowIfFailed(pipeline.device->CreateCommittedResource(
			&vertexHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&vertexResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(vertexBuffer_.ReleaseAndGetAddressOf())));
		// ���_���̃R�s�[
		Vertex* vertexMap = nullptr;
		ThrowIfFailed(vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap)));
		std::copy(std::begin(vertices), std::end(vertices), vertexMap);
		vertexBuffer_->Unmap(0, nullptr);
		// ���_�o�b�t�@�[�r���[�̐���
		vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = vertexBufferSize;
		vertexBufferView_.StrideInBytes = sizeof(Vertex);
	}

	// �C���f�b�N�X�o�b�t�@�r���[�̐���
	{
		// �C���f�b�N�X���W
		std::vector<unsigned short> indices = mesh.indices;
		const UINT indexBufferSize = sizeof(unsigned short) * indices.size();
		auto indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

		// �C���f�b�N�X�o�b�t�@�̐���
		ThrowIfFailed(pipeline.device->CreateCommittedResource(
			&indexHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&indexResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(indexBuffer_.ReleaseAndGetAddressOf())));

		// �C���f�b�N�X���̃R�s�[
		unsigned short* indexMap = nullptr;
		indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
		std::copy(std::begin(indices), std::end(indices), indexMap);
		indexBuffer_->Unmap(0, nullptr);
		// �C���f�b�N�X�o�b�t�@�r���[�̐���
		indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = indexBufferSize;
		indexBufferView_.Format = DXGI_FORMAT_R16_UINT;
	}
}

void Renderer::ApplyMaterial(const std::shared_ptr<Material>& material)
{
	this->material = material;
	hasMaterial = true;
}

void Renderer::OnDraw()
{
	material->SetPass();

	const RenderPipeline& pipeline = RenderPipeline::GetInstance();
	const ComPtr<ID3D12GraphicsCommandList>& commandList = pipeline.commandList;

	// �`�揈���̐ݒ�
	commandList->IASetPrimitiveTopology(mesh.topology); // �v���~�e�B�u�g�|���W�̐ݒ� (�O�p�|���S��)
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);                // ���_�o�b�t�@
	commandList->IASetIndexBuffer(&indexBufferView_);                         // �C���f�b�N�X�o�b�t�@

	const DirectX::XMMATRIX world = gameObject.lock()->GetComponent<Transform>()->GetMatrix();
	const DirectX::XMMATRIX viewProj = Camera::current->GetViewMatrix() * Camera::current->GetProjectionMatrix(AppInfo::GetWindowAspectRatio());

	matrixBuffer.SetBufferData({ world,viewProj });
	matrixBuffer.SetConstantBufferView(0);

	commandList->DrawIndexedInstanced(mesh.indices.size(), 1, 0, 0, 0);  // �`��
}

