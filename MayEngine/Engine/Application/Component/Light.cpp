#include "Light.h"

Light::Light() :constantLightBuffer(RenderPipeline::GetInstance().device, RenderPipeline::GetInstance().commandList)
{
}

void Light::OnDraw()
{
	const LightingData lightingData = { lightDirection,lightColor,ambientLight };

	constantLightBuffer.SetBufferData(lightingData);
	constantLightBuffer.SetConstantBufferView(1);
}

