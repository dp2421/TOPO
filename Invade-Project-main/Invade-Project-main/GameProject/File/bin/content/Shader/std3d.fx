#include "value.fx"
#include "func.fx"

#ifndef _STD3D
#define _STD3D

// ==========================
// Std3D Shader
// Deferred = true
//
// g_tex_0 : Diffuse Texture
// g_tex_1 : Normalmap Texture
// BlendState : false
// ==========================

cbuffer Parameters : register(b0)
{
    float4 sampleOffsets[16];
    float4 sampleWeights[16];
};

struct VS_STD3D_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    float4 vWeight : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;

    float3 vLights : LIGHT;
    
};

struct VS_STD3D_OUTPUT
{
    float4 vPosition : SV_Position;
    
    float3 vViewPos : POSITION;
    
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
    
    float2 vUV : TEXCOORD;
    float3 vLights : LIGHT;
};

VS_STD3D_OUTPUT VS_Std3D(VS_STD3D_INPUT _in)
{
    VS_STD3D_OUTPUT output = (VS_STD3D_OUTPUT) 0.f;

    if (g_int_0)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeight, _in.vIndices, 0); 
    }
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
    output.vUV = _in.vUV;
    output.vLights = _in.vLights;
    
    return output;
}

struct PS_STD3D_OUTPUT
{
    float4 vTarget0 : SV_Target0; // Diffuse
    float4 vTarget1 : SV_Target1; // Normal
    float4 vTarget2 : SV_Target2; // Position
};

PS_STD3D_OUTPUT PS_Std3D(VS_STD3D_OUTPUT _in)
{
    PS_STD3D_OUTPUT output = (PS_STD3D_OUTPUT)0.f;

    // ----------------------------------
    //   ** Outline Code **
    // ----------------------------------
   
    // Sobel X 커널
    float3x3 sobelX = float3x3(-1, 0, 1,
        -2, 0, 2,
        -1, 0, 1);

    // Sobel Y 커널
    float3x3 sobelY = float3x3(-1, -2, -1,
        0, 0, 0,
        1, 2, 1);
    // Sobel 커널을 사용하여 이미지의 외곽선 계산
    float textureWidth;
    float textureHeight;
    g_tex_0.GetDimensions(textureWidth, textureHeight);
    float2 texelSize = 1.0 / float2(textureWidth, textureHeight);
    float3 c00 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(-1, -1)).rgb;
    float3 c01 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(-1, 0)).rgb;
    float3 c02 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(-1, 1)).rgb;
    float3 c10 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(0, -1)).rgb;
    float3 c11 = g_tex_0.Sample(g_sam_0, _in.vUV).rgb;
    float3 c12 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(0, 1)).rgb;
    float3 c20 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(1, -1)).rgb;
    float3 c21 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(1, 0)).rgb;
    float3 c22 = g_tex_0.Sample(g_sam_0, _in.vUV + texelSize * float2(1, 1)).rgb;

    // Sobel X 커널을 사용하여 가로 방향의 엣지 검출
    float3 gx = dot(float3(c00.r, c01.r, c02.r), sobelX[0]) +
        dot(float3(c10.r, c11.r, c12.r), sobelX[1]) +
        dot(float3(c20.r, c21.r, c22.r), sobelX[2]);

    // Sobel Y 커널을 사용하여 세로 방향의 엣지 검출
    float3 gy = dot(float3(c00.r, c01.r, c02.r), sobelY[0]) +
        dot(float3(c10.r, c11.r, c12.r), sobelY[1]) +
        dot(float3(c20.r, c21.r, c22.r), sobelY[2]);

    float3 sobel = (gx * gx + gy * gy); //sqrt할수록 전체적으로 밝아짐
    output.vTarget0 = float4(sobel, 1.f);

    //

    float4 vCartoon;

    // 계산된 외곽선 값을 렌더 타겟 텍스처에 씁니다.

    // ----------------------------------
    //   ** Cartoon Shader Code **
    // ----------------------------------
   
    float brightness = dot(normalize(_in.vViewNormal), normalize(_in.vLights));
    float stepIntensity = saturate(brightness);
    float3 stepColor = float3(1, 1, 1);
        if (stepIntensity > 0.95) {
            stepColor = float3(1, 1, 1);
        }
        else if (stepIntensity > 0.5) {
            stepColor = float3(0.9, 0.9, 0.9);
        }
        else if (stepIntensity > 0.1) {
            stepColor = float3(0.7, 0.7, 0.7);
        }
        else {
            stepColor = float3(0.5, 0.5, 0.5);
        }
    if (tex_0)
    {
        float4 texColor = g_tex_0.Sample(g_sam_0, _in.vUV) * 2.f;
        //output.vTarget0 = texColor * float4(stepColor, 1.f);
        vCartoon = texColor * float4(stepColor, 1.f);
    }
    else
    {
        //output.vTarget0 = float4(stepColor, 1.f);
        ////output.vTarget0 = float4(1.f, 0.f, 1.f, 1.f);
        vCartoon = float4(stepColor, 1.f);
    }

    //output.vTarget0 = vCartoon;

    //////아래주석 풀면 텍스쳐내부 외곽라인 검은색으로 출력됨
    if (output.vTarget0.x <= 0.95f && output.vTarget0.y <= 0.95f && output.vTarget0.z <= 0.95f)
    {
        output.vTarget0 = vCartoon;
    }
    else
    {
        output.vTarget0 = float4(0, 0, 0, 1);
    }
    //// ----------------------------------

    // ------------------------------------
    //   ** Color Saturation Shader Code **
    // ------------------------------------
    float avg = (output.vTarget0.r + output.vTarget0.g + output.vTarget0.b) / 3.0f;
    float fValue = 2.0f; //0에 가까울수록 채도가 낮아짐
    float4 newColor;
    newColor.r = avg * (1.0 - fValue) + output.vTarget0.r * fValue;
    newColor.g = avg * (1.0 - fValue) + output.vTarget0.g * fValue;
    newColor.b = avg * (1.0 - fValue) + output.vTarget0.b * fValue;
    newColor.a = output.vTarget0.a;
    output.vTarget0 = newColor;


    float3 vViewNormal = _in.vViewNormal;
    // 노말맵이 있는경우
    if (tex_1)
    {
        float3 vTSNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
        float3x3 matTBN = { _in.vViewTangent, _in.vViewBinormal, _in.vViewNormal };
        vViewNormal = normalize(mul(vTSNormal, matTBN));
    }
    
    output.vTarget1.xyz = vViewNormal;
    output.vTarget2.xyz = _in.vViewPos;
    
    return output;
}


// ==========================
// Std3D Shader
// Deferred = true
//
// g_tex_0 : Diffuse Texture
// g_tex_1 : Normalmap Texture
// BlendState : false
// ==========================

VS_STD3D_OUTPUT VS_Toon3D(VS_STD3D_INPUT _in)
{
    VS_STD3D_OUTPUT output = (VS_STD3D_OUTPUT)0.f;

    if (g_int_0)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeight, _in.vIndices, 0);
    }
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
    output.vUV = _in.vUV;

    return output;
}

PS_STD3D_OUTPUT PS_Toon3D(VS_STD3D_OUTPUT _in)
{

    /*
    float4 texColor = g_Texture.Sample(g_Sampler, input.Tex);
    float3 emissive = g_EmissiveColor * texColor.rgb;

    return float4(emissive, texColor.a);
    */
    PS_STD3D_OUTPUT output = (PS_STD3D_OUTPUT)0.f;
    float brightness = dot(normalize(_in.vViewNormal), normalize(-_in.vLights));
    float stepIntensity = saturate(brightness * 10);
    float3 stepColor = float3(1, 1, 1);
        if (stepIntensity > 0.95) {
            stepColor = float3(1, 1, 1);
        }
        else if (stepIntensity > 0.5) {
            stepColor = float3(0.8, 0.8, 0.8);
        }
        else if (stepIntensity > 0.2) {
            stepColor = float3(0.6, 0.6, 0.6);
        }
        else {
            stepColor = float3(0.4, 0.4, 0.4);
        }
    if (tex_0)
    {
        float4 texColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        output.vTarget0 = texColor + float4(stepColor, 1.f);
    }
    else
    {
        output.vTarget0 = float4(stepColor, 1.f);
    }

    float3 vViewNormal = _in.vViewNormal;
    // 노말맵이 있는경우
    if (tex_1)
    {
        float3 vTSNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
        float3x3 matTBN = { _in.vViewTangent, _in.vViewBinormal, _in.vViewNormal };
        vViewNormal = normalize(mul(vTSNormal, matTBN));
    }

    output.vTarget1.xyz = vViewNormal;
    output.vTarget2.xyz = _in.vViewPos;

    return output;
}

// =============
// Skybox Shader
// mesh         : sphere
// rasterizer   : CULL_FRONT
// DepthStencilState : Less_Equal
// g_tex_0 : Output Texture
// =============
struct VS_SKY_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_SKY_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VS_SKY_OUT VS_Skybox(VS_SKY_IN _in)
{
    VS_SKY_OUT output = (VS_SKY_OUT) 0.f;
    
    float4 vViewPos = mul(float4(_in.vPos, 0.f), g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);
    
    // w 값으로 z 값을 나눌것이기 때문에 미리 w 값을 셋팅해두면
    // 어떤 상황에서도 깊이값이 1.f 로 판정된다.
    vProjPos.z = vProjPos.w;
    
    output.vPosition = vProjPos;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Skybox(VS_SKY_OUT _in) : SV_Target
{
    float4 vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    return vOutColor;
}


#endif