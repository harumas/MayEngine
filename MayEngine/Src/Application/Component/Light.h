#pragma once
#include "Component.h"
#include "ConstantBuffer.h"
#include "../RenderPipeline.h"

class Light : public Object
{
public:
	Light();

	// ライティング用
	DirectX::XMVECTOR ambientLight = { 0.35f, 0.35f, 0.35f };
	DirectX::XMVECTOR lightColor = { 0.8f, 0.8f, 1.0f };
	DirectX::XMVECTOR lightDirection = { -0.3f, -0.3f, 0.8f };

	void OnDraw() override;
private:
	struct LightingData
	{
		DirectX::XMVECTOR ambientLight;
		DirectX::XMVECTOR lightColor;
		DirectX::XMVECTOR lightDirection;
	};

	ConstantBuffer<LightingData> constantLightBuffer;
};
