//// Á¤Á¡ ¼ÎÀÌ´õ
//cbuffer VSConstantBuffer : register(b0)
//{
//    matrix worldViewProjectionMatrix;
//};
//

#include "value.fx"
#include "func.fx"

struct VS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION;
    float3 worldNormal : NORMAL;
};

//VSOutput VS_Outline(VSInput _in)
//{
//    VS_Output output = (VS_Output)0.f;
//
//    output.position = mul(float4(_in.position, 1.0f), g_matWVP);
//    output.worldPosition = _in.position;
//    output.worldNormal = _in.normal;
//    return output;
//}
VS_Output VSOutline(VS_Input _in)
{
    VS_Output output = (VS_Output)0.f;
    output.position = mul(float4(_in.position, 1.0f), g_matWVP);
    float fScale = 1.00175f;
    //float fScale = 1.00175f + length(gvCameraPosition - output.positionW) * 0.00035f;
    output.worldPosition = (float3)mul(float4(_in.position * fScale, 1.0f), g_matWVP);
    output.position = mul(mul(float4(output.worldPosition, 1.0f), g_matView), g_matProj);

    return(output);
}
//// ÇÈ¼¿ ¼ÎÀÌ´õ
//cbuffer PSConstantBuffer : register(b0)
//{
//    float4x4 viewMatrix;
//    float4x4 projectionMatrix;
//    float3 cameraPosition;
//    float outlineThickness;
//};

struct PS_Input
{
    float3 worldPosition : POSITION;
    float3 worldNormal : NORMAL;
};

//float4 main(PSInput input) : SV_Target
//{
//    float3 viewDirection = normalize(cameraPosition - input.worldPosition);
//    float3 outlineColor = float3(1.0f, 0.0f, 0.0f); // »¡°£»ö À±°û¼±
//
//    // °æ°è¸é ¹ý¼± º¤ÅÍ¿Í ºä º¤ÅÍÀÇ ¿ÜÀûÀ» °è»êÇÏ¿© ºûÀÇ ¹æÇâÀ» ±¸ÇÔ
//    float3 worldNormal = normalize(input.worldNormal);
//    float3 lightDirection = normalize(cross(worldNormal, viewDirection));
//
//    // À±°û¼± »ö»ó °è»ê
//    float outlineDot = dot(viewDirection, lightDirection);
//    float outlineAlpha = saturate(outlineDot - 1.0f + outlineThickness);
//    float3 outlineFinalColor = outlineColor * outlineAlpha;
//
//    return float4(outlineFinalColor, outlineAlpha);
//}
//


float4 PSOutline(PS_Input input) :SV_Target
{
    return(float4(1.0f, 0.2f, 0.2f, 0.0f));
}
