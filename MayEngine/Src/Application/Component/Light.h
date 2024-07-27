#pragma once
#include "Component.h"
#include "../RenderPipeline.h"
#include "../../../ConstantBuffer.h"

class Light : Object
{
public:
	// ライティング用
	DirectX::XMVECTOR ambientLight = { 0.35f, 0.35f, 0.35f };
	DirectX::XMVECTOR lightColor = { 0.8f, 0.8f, 1.0f };
	DirectX::XMVECTOR lightDirection = { 0.3f, 0.3f, 0.8f };

	void OnDraw() override;
private:
	Light();

	struct LightingData
	{
		DirectX::XMVECTOR ambientLight;
		DirectX::XMVECTOR lightColor;
		DirectX::XMVECTOR lightDirection;
	};

	ConstantBuffer<LightingData> constantLightBuffer;
};
