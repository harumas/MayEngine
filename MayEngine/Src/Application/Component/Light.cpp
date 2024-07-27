#include "Light.h"

Light::Light() :constantLightBuffer(RenderPipeline::instance->device_, RenderPipeline::instance->commandList_)
{
}

void Light::OnDraw()
{
	const LightingData lightingData = { ambientLight,lightColor,lightDirection };

	constantLightBuffer.SetBufferData(lightingData);
	constantLightBuffer.SetConstantBufferView(1);
}

