struct DirectionalLight
{
    float3 direction;
    float3 color;
    float3 ambientLight;
};

struct PointLight
{
    float3 position;
    float3 color;
    float range;
};

cbuffer cbuff0 : register(b0)
{
    matrix world;
    matrix viewproj;
}

cbuffer cbuff1 : register(b1)
{
    DirectionalLight directionalLight;
}

cbuffer cbuff2 : register(b2)
{
    PointLight pointLight;
}

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal);

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
    float3 directionNormal = normalize(directionalLight.direction);
    float3 diffuseDirection = CalcLambertDiffuse(directionNormal, directionalLight.color, input.normal);
     
    float3 pointDir = normalize(input.position - pointLight.position);

    float3 diffPoint = CalcLambertDiffuse(
        pointDir, // ライトの方向
        pointLight.color, // ライトのカラー
        input.normal // サーフェイスの法線
    );

	// step-10 距離による影響率を計算する
    // ポイントライトとの距離を計算する
    float3 distance = length(input.position - pointLight.position);

    // 影響率は距離に比例して小さくなっていく
    float affect = 1.0f - 1.0f / pointLight.range * distance;

    //影響を指数関数的にする。今回のサンプルでは3乗している
    affect = pow(max(0.0f, affect), 3.0f);

    // step-11 拡散反射光と鏡面反射光に減衰率を乗算して影響を弱める
    diffPoint *= affect;

    // step-12 2つの反射光を合算して最終的な反射光を求める
    float3 diffuseLig = diffPoint + diffuseDirection;

    // 拡散反射光と鏡面反射光を足し算して、最終的な光を求める
    float3 light = diffuseLig + directionalLight.ambientLight;
    float4 finalColor = float4(g_texture.Sample(g_sampler, input.uv) * light, 1);

    return finalColor;
}

float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, -lightDirection);

    // 内積の値を0以上の値にする
    t = max(0.0f, t);

    // 拡散反射光を計算する
    return lightColor * t;
}


