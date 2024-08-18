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

cbuffer cbuff1 : register(b2)
{
    PointLight pointLight;
}
 
cbuffer cbuff2 : register(b3)
{
    float3 baseColor;
    float3 eyePos;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
};

float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal);
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal);

PSInput VSMain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    PSInput result;
    result.worldPos = mul(world, pos);
    result.position = mul(viewproj, result.worldPos);
    result.normal = normalize(mul(world, normal));
    result.uv = uv;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 directionNormal = normalize(directionalLight.direction);
    float3 diffuseDirection = CalcLambertDiffuse(directionNormal, directionalLight.color, input.normal);
    float3 specularDirection = CalcPhongSpecular(directionNormal, directionalLight.color, input.position, input.normal);
     
    float3 lightDir = normalize(input.worldPos - pointLight.position);
    float3 distance = length(input.worldPos - pointLight.position);

    float3 diffusePoint = pointLight.color * max(0.0f, dot(input.normal, -lightDir));

    // step-9 �����Ȃ���Phong���ʔ��ˌ����v�Z����
    float3 specPoint = CalcPhongSpecular(
        lightDir, // ���C�g�̕���
        pointLight.color, // ���C�g�̃J���[
        input.position, // �T�[�t�F�C�Y�̃��[���h���W
        input.normal     // �T�[�t�F�C�Y�̖@��
    );

	// �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
    //float affect = (1.0f - distance / pointLight.range) / (1.0f + distance * a * a);
    float affect = 1.0f / max(0.01f, distance * distance);

    // step-11 �g�U���ˌ��Ƌ��ʔ��ˌ��Ɍ���������Z���ĉe������߂�
    diffusePoint *= affect;

 
    // step-11 �g�U���ˌ��Ƌ��ʔ��ˌ��Ɍ���������Z���ĉe������߂�
    diffusePoint *= affect;
    specPoint *= affect;

    // step-12 2�̔��ˌ������Z���čŏI�I�Ȕ��ˌ������߂�
    float3 diffuseLig = diffusePoint + diffuseDirection;
    float3 specularLig = specPoint + specularDirection;

    // �g�U���ˌ��Ƌ��ʔ��ˌ��𑫂��Z���āA�ŏI�I�Ȍ������߂�
    float3 light = diffuseLig + specularLig + directionalLight.ambientLight;
    float4 finalColor = float4(baseColor * light, 1);

    return finalColor;
}


float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
    float t = dot(normal, -lightDirection);

    t = t * 0.5f + 0.5;
    t = t * t;

    // �g�U���ˌ����v�Z����
    return lightColor * t;
}


/// <summary>
/// Phong���ʔ��ˌ����v�Z����
/// </summary>
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // ���˃x�N�g�������߂�
    float3 refVec = reflect(lightDirection, normal);

    // �������������T�[�t�F�C�X���王�_�ɐL�т�x�N�g�������߂�
    float3 toEye = eyePos - worldPos;
    toEye = normalize(toEye);

    // ���ʔ��˂̋��������߂�
    float t = dot(refVec, toEye);

    // ���ʔ��˂̋�����0�ȏ�̐��l�ɂ���
    t = max(0.0f, t);

    // ���ʔ��˂̋������i��
    t = pow(t, 3.0f);

    // ���ʔ��ˌ������߂�
    return lightColor * t;
}

