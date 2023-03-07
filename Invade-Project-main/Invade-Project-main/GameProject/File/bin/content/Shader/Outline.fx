// Á¤Á¡ ¼ÎÀÌ´õ
cbuffer VSConstantBuffer : register(b0)
{
    matrix worldViewProjectionMatrix;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 worldNormal : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), worldViewProjectionMatrix);
    output.worldPosition = input.position;
    output.worldNormal = input.normal;
    return output;
}

// ÇÈ¼¿ ¼ÎÀÌ´õ
cbuffer PSConstantBuffer : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3 cameraPosition;
    float outlineThickness;
};

struct PSInput
{
    float3 worldPosition : TEXCOORD0;
    float3 worldNormal : TEXCOORD1;
};

float4 main(PSInput input) : SV_Target
{
    float3 viewDirection = normalize(cameraPosition - input.worldPosition);
    float3 outlineColor = float3(1.0f, 0.0f, 0.0f); // »¡°£»ö À±°û¼±

    // °æ°è¸é ¹ý¼± º¤ÅÍ¿Í ºä º¤ÅÍÀÇ ¿ÜÀûÀ» °è»êÇÏ¿© ºûÀÇ ¹æÇâÀ» ±¸ÇÔ
    float3 worldNormal = normalize(input.worldNormal);
    float3 lightDirection = normalize(cross(worldNormal, viewDirection));

    // À±°û¼± »ö»ó °è»ê
    float outlineDot = dot(viewDirection, lightDirection);
    float outlineAlpha = saturate(outlineDot - 1.0f + outlineThickness);
    float3 outlineFinalColor = outlineColor * outlineAlpha;

    return float4(outlineFinalColor, outlineAlpha);
}
