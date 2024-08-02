cbuffer cbuff0 : register(b0)
{
    matrix world;
    matrix viewproj;
}

cbuffer cbuff1 : register(b1)
{
    float3 ambientLight;
    float3 lightColor;
    float3 lightDirection;
}
 
cbuffer cbuff2 : register(b2)
{
    float3 baseColor;
    float3 eyePos;
}

SamplerState g_sampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

PSInput VSMain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    PSInput result;
    result.position = mul(mul(viewproj, world), pos);
    result.normal = mul(world, normal);
    result.uv = uv;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // ピクセルの法線とライトの方向の内積を計算する  
    float t = dot(input.normal, normalize(lightDirection));
    t *= -1;
     
    // 内積の結果が0以下なら0にする
    if (t < 0.0f)
    {
        t = 0.0f;
    }
     
    // 拡散反射光を求める
    float3 diffuseLight = lightColor * t;
     
    // 反射ベクトルを求める 
    float3 reflectVec = reflect(normalize(lightDirection), input.normal);
     
    //光があたったサーフェイスから視点に伸びるベクトルを求める 
    float3 toEye = normalize(eyePos - input.position);
     
     
     
    t = dot(reflectVec, toEye);
    if (t < 0.0f)
    {
        t = 0.0f;
    }
     
    // 鏡面反射の強さを求める 
    t = pow(t, 1.0);
     
    // 鏡面反射光を求める 
    float3 specularLight = lightColor * t;
    float3 light = diffuseLight + specularLight + ambientLight;
     
    return  float4(baseColor * light, 1);
}
