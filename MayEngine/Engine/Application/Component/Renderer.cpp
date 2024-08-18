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
	// fbxモデルのロード
	if (!FbxLoader::Load(filePath, &mesh))
	{
		ThrowMessage("failed load fbx file.");
	}

	//三角ポリゴン
	mesh.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	const RenderPipeline& pipeline = RenderPipeline::GetInstance();

	// 頂点バッファビューの生成
	{
		// 頂点座標
		std::vector<Vertex> vertices = mesh.vertices;
		const UINT vertexBufferSize = sizeof(Vertex) * vertices.size();
		auto vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto vertexResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		// 頂点バッファーの生成
		ThrowIfFailed(pipeline.device->CreateCommittedResource(
			&vertexHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&vertexResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(vertexBuffer_.ReleaseAndGetAddressOf())));
		// 頂点情報のコピー
		Vertex* vertexMap = nullptr;
		ThrowIfFailed(vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap)));
		std::copy(std::begin(vertices), std::end(vertices), vertexMap);
		vertexBuffer_->Unmap(0, nullptr);
		// 頂点バッファービューの生成
		vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = vertexBufferSize;
		vertexBufferView_.StrideInBytes = sizeof(Vertex);
	}

	// インデックスバッファビューの生成
	{
		// インデックス座標
		std::vector<unsigned short> indices = mesh.indices;
		const UINT indexBufferSize = sizeof(unsigned short) * indices.size();
		auto indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

		// インデックスバッファの生成
		ThrowIfFailed(pipeline.device->CreateCommittedResource(
			&indexHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&indexResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(indexBuffer_.ReleaseAndGetAddressOf())));

		// インデックス情報のコピー
		unsigned short* indexMap = nullptr;
		indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
		std::copy(std::begin(indices), std::end(indices), indexMap);
		indexBuffer_->Unmap(0, nullptr);
		// インデックスバッファビューの生成
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

	// 描画処理の設定
	commandList->IASetPrimitiveTopology(mesh.topology); // プリミティブトポロジの設定 (三角ポリゴン)
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);                // 頂点バッファ
	commandList->IASetIndexBuffer(&indexBufferView_);                         // インデックスバッファ

	const DirectX::XMMATRIX world = gameObject.lock()->GetComponent<Transform>()->GetMatrix();
	const DirectX::XMMATRIX viewProj = Camera::current->GetViewMatrix() * Camera::current->GetProjectionMatrix(AppInfo::GetWindowAspectRatio());

	matrixBuffer.SetBufferData({ world,viewProj });
	matrixBuffer.SetConstantBufferView(0);

	commandList->DrawIndexedInstanced(mesh.indices.size(), 1, 0, 0, 0);  // 描画
}

