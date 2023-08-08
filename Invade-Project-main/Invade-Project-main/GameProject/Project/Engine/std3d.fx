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
   
    // Sobel X Ŀ��
    float3x3 sobelX = float3x3(-1, 0, 1,
        -2, 0, 2,
        -1, 0, 1);

    // Sobel Y Ŀ��
    float3x3 sobelY = float3x3(-1, -2, -1,
        0, 0, 0,
        1, 2, 1);
    // Sobel Ŀ���� ����Ͽ� �̹����� �ܰ��� ���
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

    // Sobel X Ŀ���� ����Ͽ� ���� ������ ���� ����
    float3 gx = dot(float3(c00.r, c01.r, c02.r), sobelX[0]) +
        dot(float3(c10.r, c11.r, c12.r), sobelX[1]) +
        dot(float3(c20.r, c21.r, c22.r), sobelX[2]);

    // Sobel Y Ŀ���� ����Ͽ� ���� ������ ���� ����
    float3 gy = dot(float3(c00.r, c01.r, c02.r), sobelY[0]) +
        dot(float3(c10.r, c11.r, c12.r), sobelY[1]) +
        dot(float3(c20.r, c21.r, c22.r), sobelY[2]);

    float3 sobel = (gx * gx + gy * gy); //sqrt�Ҽ��� ��ü������ �����
    output.vTarget0 = float4(sobel, 1.f);

    //

    float4 vCartoon;

    // ���� �ܰ��� ���� ���� Ÿ�� �ؽ�ó�� ���ϴ�.

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

    //////�Ʒ��ּ� Ǯ�� �ؽ��ĳ��� �ܰ����� ���������� ��µ�
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
    float fValue = 2.0f; //0�� �������� ä���� ������
    float4 newColor;
    newColor.r = avg * (1.0 - fValue) + output.vTarget0.r * fValue;
    newColor.g = avg * (1.0 - fValue) + output.vTarget0.g * fValue;
    newColor.b = avg * (1.0 - fValue) + output.vTarget0.b * fValue;
    newColor.a = output.vTarget0.a;
    output.vTarget0 = newColor;


    float3 vViewNormal = _in.vViewNormal;
    // �븻���� �ִ°��
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

// ================================================================================
// ================================================================================
// effect Shader
 
// for rainbow
float3 HSVtoRGB(float3 HSV)
{
    float3 RGB = 0;
    float C = HSV.z * HSV.y;
    float H = HSV.x * 6;
    float X = C * (1 - abs(fmod(H, 2) - 1));
    if (HSV.y != 0)
    {
        float I = floor(H);
        if (I == 0) { RGB = float3(C, X, 0); }
        else if (I == 1) { RGB = float3(X, C, 0); }
        else if (I == 2) { RGB = float3(0, C, X); }
        else if (I == 3) { RGB = float3(0, X, C); }
        else if (I == 4) { RGB = float3(X, 0, C); }
        else { RGB = float3(C, 0, X); }
    }
    float M = HSV.z - C;
    return RGB + M;
}

float3 RGBtoHSV(float3 RGB)
{
    float3 HSV = 0;
    float M = min(RGB.r, min(RGB.g, RGB.b));
    HSV.z = max(RGB.r, max(RGB.g, RGB.b));
    float C = HSV.z - M;
    if (C != 0)
    {
        HSV.y = C / HSV.z;
        float3 D = (((HSV.z - RGB) / 6) + (C / 2)) / C;
        if (RGB.r == HSV.z)
            HSV.x = D.b - D.g;
        else if (RGB.g == HSV.z)
            HSV.x = (1.0 / 3.0) + D.r - D.b;
        else if (RGB.b == HSV.z)
            HSV.x = (2.0 / 3.0) + D.g - D.r;
        if (HSV.x < 0.0) { HSV.x += 1.0; }
        if (HSV.x > 1.0) { HSV.x -= 1.0; }
    }
    return HSV;
}

PS_STD3D_OUTPUT PS_Bloom(VS_STD3D_OUTPUT _in)
{
    PS_STD3D_OUTPUT output = (PS_STD3D_OUTPUT)0.f;

    // ----------------------------------
    //   ** Cartoon Shader Code **
    // ----------------------------------
    float4 vCartoon;
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

    output.vTarget0 = vCartoon;

    // ------------------------------------
    //   ** Color Saturation Shader Code **
    // ------------------------------------
    float avg = (output.vTarget0.r + output.vTarget0.g + output.vTarget0.b) / 3.0f;
    float fValue = 2.0f; //0�� �������� ä���� ������
    float4 newColor;
    newColor.r = avg * (1.0 - fValue) + output.vTarget0.r * fValue;
    newColor.g = avg * (1.0 - fValue) + output.vTarget0.g * fValue;
    newColor.b = avg * (1.0 - fValue) + output.vTarget0.b * fValue;
    newColor.a = output.vTarget0.a;
    output.vTarget0 = newColor;

    //=============================================
    //Test Effect - rainbow
    //float4 Color = g_tex_0.Sample(g_sam_0, _in.vUV);
    //float2 temp = _in.vUV;
    //float3 hsv = RGBtoHSV(Color.rgb);
    //hsv.x = frac(hsv.x + 0.05 * temp.x);
    //Color.rgb = HSVtoRGB(hsv);
    //output.vTarget0 = float4(Color);


    float4 red = float4(1, 0, 0, 1);
    float4 orange = float4(1, .5, 0, 1);
    float4 yellow = float4(1, 1, 0, 1);
    float4 green = float4(0, 1, 0, 1);
    float4 blue = float4(0, 0, 1, 1);
    float4 indigo = float4(.3, 0, .8, 1);
    float4 violet = float4(1, .8, 1, 1);
    float4 black = float4(0, 0, 0, 1);

    float step = 1.0 / 7;

    float4 color = g_tex_0.Sample(g_sam_0, _in.vUV);

    if (!any(color))
        output.vTarget0 = color;

    if (_in.vUV.x < (step * 1)) color = red;
    else if (_in.vUV.x < (step * 2)) color = orange;
    else if (_in.vUV.x < (step * 3)) color = yellow;
    else if (_in.vUV.x < (step * 4)) color = green;
    else if (_in.vUV.x < (step * 5)) color = blue;
    else if (_in.vUV.x < (step * 6)) color = indigo;
    else                            color = violet;

    output.vTarget0 = color;

    //=============================================

    float3 vViewNormal = _in.vViewNormal;
    // �븻���� �ִ°��
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
// ================================================================================
// ================================================================================
// UI Shader
PS_STD3D_OUTPUT PS_UI(VS_STD3D_OUTPUT _in)
{
    PS_STD3D_OUTPUT output = (PS_STD3D_OUTPUT)0.f;

    float4 vCartoon;

    // ���� �ܰ��� ���� ���� Ÿ�� �ؽ�ó�� ���ϴ�.

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

    output.vTarget0 = vCartoon;

    ////////�Ʒ��ּ� Ǯ�� �ؽ��ĳ��� �ܰ����� ���������� ��µ�
    //if (output.vTarget0.x <= 0.95f && output.vTarget0.y <= 0.95f && output.vTarget0.z <= 0.95f)
    //{
    //    output.vTarget0 = vCartoon;
    //}
    //else
    //{
    //    output.vTarget0 = float4(0, 0, 0, 1);
    //}
    //// ----------------------------------

    // ------------------------------------
    //   ** Color Saturation Shader Code **
    // ------------------------------------
    float avg = (output.vTarget0.r + output.vTarget0.g + output.vTarget0.b) / 3.0f;
    float fValue = 2.0f; //0�� �������� ä���� ������
    float4 newColor;
    newColor.r = avg * (1.0 - fValue) + output.vTarget0.r * fValue;
    newColor.g = avg * (1.0 - fValue) + output.vTarget0.g * fValue;
    newColor.b = avg * (1.0 - fValue) + output.vTarget0.b * fValue;
    newColor.a = output.vTarget0.a;
    output.vTarget0 = newColor;


    float3 vViewNormal = _in.vViewNormal;
    // �븻���� �ִ°��
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


// ================================================================================
// ================================================================================
// for Goal FBX Shader
PS_STD3D_OUTPUT PS_RED(VS_STD3D_OUTPUT _in)
{
    PS_STD3D_OUTPUT output = (PS_STD3D_OUTPUT)0.f;

    // ----------------------------------
    //   ** Cartoon Shader Code **
    // ----------------------------------
    float4 vCartoon;
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

    output.vTarget0 = vCartoon;

    // ------------------------------------
    //   ** Color Saturation Shader Code **
    // ------------------------------------
    float avg = (output.vTarget0.r + output.vTarget0.g + output.vTarget0.b) / 3.0f;
    float fValue = 2.0f; //0�� �������� ä���� ������
    float4 newColor;
    newColor.r = avg * (1.0 - fValue) + output.vTarget0.r * fValue;
    newColor.g = avg * (1.0 - fValue) + output.vTarget0.g * fValue;
    newColor.b = avg * (1.0 - fValue) + output.vTarget0.b * fValue;
    newColor.a = output.vTarget0.a;
    output.vTarget0 = newColor;

    //=============================================
    //Test Effect - rainbow
    //float4 Color = g_tex_0.Sample(g_sam_0, _in.vUV);
    //float2 temp = _in.vUV;
    //float3 hsv = RGBtoHSV(Color.rgb);
    //hsv.x = frac(hsv.x + 0.05 * temp.x);
    //Color.rgb = HSVtoRGB(hsv);
    //output.vTarget0 = float4(Color);

    float4 color = g_tex_0.Sample(g_sam_0, _in.vUV);

    float4 red = float4(1, 0, 0, 1);
    float4 orange = float4(1, .5, 0, 1);
    float4 yellow = float4(1, 1, 0, 1);
    float4 green = float4(0, 1, 0, 1);
    float4 blue = float4(0, 0, 1, 1);
    float4 indigo = float4(.3, 0, .8, 1);
    float4 violet = float4(1, .8, 1, 1);
    float4 black = float4(0, 0, 0, 1);

    //float step = 1.0 / 7;

    if (!any(color))
        output.vTarget0 = color;

    color = red;
    output.vTarget0 = color;

    //=============================================

    float3 vViewNormal = _in.vViewNormal;
    // �븻���� �ִ°��
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
    // �븻���� �ִ°��
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
    
    // w ������ z ���� �������̱� ������ �̸� w ���� �����صθ�
    // � ��Ȳ������ ���̰��� 1.f �� �����ȴ�.
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