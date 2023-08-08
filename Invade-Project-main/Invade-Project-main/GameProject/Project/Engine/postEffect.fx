#ifndef _POSTEFFECT
#define _POSTEFFECT

#include "value.fx"
#include "func.fx"

static const int MAX_SAMPLES = 16;


Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);


cbuffer Parameters : register(b0)
{
    float4 sampleOffsets[MAX_SAMPLES];
    float4 sampleWeights[MAX_SAMPLES];
};


struct VS_INPUT
{
    float3 vPos : POSITION; // sementic (지시자) 정점 Layout 과 연동       
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 vOutPos : SV_Position;
    float4 vOutColor : COLOR;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    float3 vViewPos : POSITION;
    float2 vUV : TEXCOORD;
};


// Vertex shader: self-created quad.

VS_OUTPUT VSQuad(uint vI : SV_VertexId)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // We use the 'big triangle' optimization so you only Draw 3 verticies instead of 4.
    float2 texcoord = float2((vI << 1) & 2, vI & 2);
    output.vUV = texcoord;

    output.vOutPos = float4(texcoord.x * 2 - 1, -texcoord.y * 2 + 1, 0, 1); //-1~1 사이로 정규화
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel shader: copy.

float4 PSCopy(VS_OUTPUT _input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, _input.vUV);
    return color;
}


// Pixel shader: monochrome.

float4 PSMonochrome(VS_OUTPUT _input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, _input.vUV);
    float3 grayscale = float3(0.2125f, 0.7154f, 0.0721f);
    float3 output = dot(color.rgb, grayscale);
    return float4(output, color.a);
}


// Pixel shader: sepia.

float4 PSSepia(VS_OUTPUT _input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, _input.vUV);

    float3 red = float3(0.393f, 0.769f, 0.189f);
    float3 green = float3(0.349f, 0.686f, 0.168f);
    float3 blue = float3(0.272f, 0.534f, 0.131f);

    float3 output;
    output.r = dot(color.rgb, red);
    output.g = dot(color.rgb, green);
    output.b = dot(color.rgb, blue);
    return float4(output, color.a);
}




#endif 