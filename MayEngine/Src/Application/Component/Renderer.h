#pragma once
#include "Component.h"
#include "../RenderPipeline.h"
#include "../../../ConstantBuffer.h"

class Renderer : public Component
{
public:
	explicit Renderer(const std::shared_ptr<GameObject>& gameObjectPtr);

	void LoadMesh(const std::string& filePath);
	void Draw();

private:
	Mesh mesh;

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	ConstantBuffer<DirectX::XMMATRIX> modelMatrixBuffer;
};
