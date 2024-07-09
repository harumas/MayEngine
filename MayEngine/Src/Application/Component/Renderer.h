#pragma once
#include "Component.h"
#include "../RenderPipeline.h"

class Renderer : public Component
{
public:
	explicit Renderer(const std::shared_ptr<GameObject>& gameObjectPtr)
		: Component(gameObjectPtr)
	{
		if (auto pipeline = RenderPipeline::instance)
		{
			matrixHandle = pipeline->AssignBuffer();
			Transform transform = gameObjectPtr->GetComponent<Transform>();
			pipeline->CreateMatrixBufferView(matrixHandle, gameObject.lock()->transform->GetMatrix());
		}
	}

	void LoadMesh(const std::string& filePath);
	void Draw();

	int matrixHandle;

private:
	Mesh mesh;

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
};
