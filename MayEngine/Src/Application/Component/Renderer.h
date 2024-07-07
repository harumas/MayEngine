#pragma once
#include "Component.h"

class Renderer : Component
{
public:
	explicit Renderer(const shared_ptr<GameObject>& gameObjectPtr)
		: Component(gameObjectPtr)
	{
	}

	void LoadMesh(const string& filePath);
	void Draw();

private:
	Mesh mesh;
	int matrixHandle;

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
};
