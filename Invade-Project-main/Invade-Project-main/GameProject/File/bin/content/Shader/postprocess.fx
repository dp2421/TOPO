static const int MAX_SAMPLES = 16;


Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);


cbuffer Parameters : register(b0)
{
    float4 sampleOffsets[MAX_SAMPLES];
    float4 sampleWeights[MAX_SAMPLES];
};

#include "value.fx"
#include "func.fx"

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

    output.Position = float4(texcoord.x * 2 - 1, -texcoord.y * 2 + 1, 0, 1);
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


// Pixel shader: down-sample 2x2.

float4 PSDownScale2x2(VS_OUTPUT _input) : SV_Target
{
    const int NUM_SAMPLES = 4;
    float4 vColor = 0.0f;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        vColor += g_tex_0.Sample(g_sam_0, _input.vUV + sampleOffsets[i].xy);
    }

    return vColor / NUM_SAMPLES;
}


// Pixel shader: down-sample 4x4.

float4 PSDownScale4x4(VS_OUTPUT _input) : SV_Target
{
    const int NUM_SAMPLES = 16;
    float4 vColor = 0.0f;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        vColor += g_tex_0.Sample(g_sam_0, _input.vUV + sampleOffsets[i].xy);
    }

    return vColor / NUM_SAMPLES;
}


// Pixel shader: gaussian blur 5x5.

float4 PSGaussianBlur5x5(VS_OUTPUT _input) : SV_Target
{
    float4 vColor = 0.0f;

    for (int i = 0; i < 13; i++)
    {
        vColor += sampleWeights[i] * g_tex_0.Sample(g_sam_0, _input.vUV + sampleOffsets[i].xy);
    }

    return vColor;
}


// Pixel shader: bloom (extract)

float4 PSBloomExtract(VS_OUTPUT _input) : SV_Target
{
    // Uses sampleWeights[0] as 'bloom threshold'
    float4 c = g_tex_0.Sample(g_sam_0, _input.vUV);
    return saturate((c - sampleWeights[0]) / (1 - sampleWeights[0]));
}


// Pixel shader: bloom (blur)

float4 PSBloomBlur(VS_OUTPUT _input) : SV_Target
{
    float4 vColor = 0.0f;

    // Perform a one-directional gaussian blur
    for (int i = 0; i < 15; i++)
    {
        vColor += sampleWeights[i] * g_tex_0.Sample(g_sam_0, _input.vUV + sampleOffsets[i].xy);
    }

    return vColor;
}