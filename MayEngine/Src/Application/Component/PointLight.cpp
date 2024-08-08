#include "PointLight.h"

PointLight::PointLight() :constantLightBuffer(RenderPipeline::instance->device_, RenderPipeline::instance->commandList_)
{
}

void PointLight::OnDraw()
{
	const LightingData lightingData = { position, lightColor, lightRange };

	constantLightBuffer.SetBufferData(lightingData);
	constantLightBuffer.SetConstantBufferView(2);
}

