#include "PointLight.h"

PointLight::PointLight() :constantLightBuffer(RenderPipeline::GetInstance().device, RenderPipeline::GetInstance().commandList)
{
}

void PointLight::OnDraw()
{
	LightingData lightingData;
	lightingData.lightPosition = position;
	lightingData.lightColor = lightColor;
	lightingData.range = lightRange;

	constantLightBuffer.SetBufferData(lightingData);
	constantLightBuffer.SetConstantBufferView(2);
}

