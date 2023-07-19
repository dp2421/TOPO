
#include "value.fx"
#include "func.fx"

#ifndef _MOTIONBLUR
#define _MOTIONBLUR

float4x4 m_WVPNew;             // ���� ���� ������ ���� x �� x ����
float4x4 m_WVPOld;             // ���� ���� ������ ���� x �� x �翵 
float4x4 m_ROnly;               // ���� �������� ���� ���͸� ��ȯ�ϱ� ���� ���. ���� �����̹Ƿ� ȸ�� ���и�. 

sampler tex0 : register(s0);   // ����� ������ �̹���. Pass1���� ����Ѵ�. 
sampler tex1 : register(s1);   // �ӵ� ��. Pass1���� ����Ѵ�. 

// �ӵ� �� ���� 
struct VS_CreateVMap_OUTPUT
{
    float4 Pos      : POSITION;
    float4 Dir      : TEXCOORD0;   // �ӵ� ���� 
};

VS_CreateVMap_OUTPUT VS_CreateVMap(float4 Pos     : POSITION,
                                    float4 Normal : NORMAL,
                                    float2 Tex : TEXCOORD0)
{
    VS_CreateVMap_OUTPUT Out;

    // ���� ���ʹ� ȸ�� ���и��� ����� ���� 
    float3 N = mul(Normal.xyz, m_ROnly);

    // ���� ������ ��ǥ 
    float4 NewPos = mul(Pos, m_WVPNew);

    // ���� ��ǥ
    float4 OldPos = mul(Pos, m_WVPOld);

    // ������ �̵� ���� ���� 
    float3 Dir = NewPos.xyz - OldPos.xyz;

    //������ �̵� ���� ���Ϳ� ������ ������ ������ ��� 
    float a = dot(normalize(Dir), normalize(N));

    //���� �������� ���� �ø��� 
    if (a < 0.0f)
        Out.Pos = OldPos;
    else
        Out.Pos = NewPos;

    // �ӵ� ���� ��� 
    //-1.0 f ���� 1.0f �� ������ �ؽ�ó ��ǥ���� 0.0f ~ 1.0f �� �ؼ� ��ġ�� �����ϱ� ���� �Ÿ��� ������ �Ѵ�
    Out.Dir.xy = (NewPos.xy / NewPos.w - OldPos.xy / OldPos.w) * 0.5f;

    // ���������� �ؼ��� ������ ���� �Ǳ� ������ Y ������ ���������� �Ѵ�
    Out.Dir.y *= -1.0f;

    // ����� Z ���� ����ϱ� ���� �Ű� ���� 
    Out.Dir.z = Out.Pos.z;
    Out.Dir.w = Out.Pos.w;

    return Out;
}

float4 PS_CreateVMap(VS_CreateVMap_OUTPUT In) : COLOR0
{
   float4 Out;

    // �ӵ� ���� 
    Out.xy = In.Dir.xy;

    // �̻�� 
    Out.z = 1.0f;

    // Z �� ��� 
    Out.w = In.Dir.z / In.Dir.w;

    return Out;
}

// �ӵ� �� �� �����Ͽ� �帲 ó��
struct VS_Final_OUTPUT
{
    float4 Pos   : POSITION;
    float2 Tex   : TEXCOORD0;
};

VS_Final_OUTPUT VS_Final(float4 Pos     : POSITION,
                        float4 COLOR0 : COLOR0,
                        float2 Tex : TEXCOORD0)
{
    VS_Final_OUTPUT Out;

    Out.Pos = Pos;
    Out.Tex = Tex;

    return Out;
}

float4 PS_Final(VS_Final_OUTPUT In) : COLOR0
{

    // ������ �Ų�����. ��ġ�� ũ�� �ϸ� �ε巯������. 
    int Blur = 10;

    // �ӵ� �ʿ��� �ӵ� ���Ϳ� Z ���� �����ɴϴ�. 
    float4 Velocity = tex2D(tex1, In.Tex);

    Velocity.xy /= (float)Blur;

    int cnt = 1;
    float4 BColor;

    // ��� ������ �̹����� �����ɴϴ�. a ���п� Z ���� ����ȴ�. 
    float4 Out = tex2D(tex0, In.Tex);

    for (int i = cnt; i < Blur; i++)
    {
        // �ӵ� ���� ������ �ؼ� ��ġ�� �����Ͽ� ��� ������ �̹����� �� ������ �����ɴϴ�.
        BColor = tex2D(tex0, In.Tex + Velocity.xy * (float)i);

        // �ӵ� ���� Z���� �ӵ� ���� ���⿡ �ִ� �ؼ� ��ġ�� ���ø��� ����� ������ �̹����� Z���� ���Ѵ�. (���� 1) 
        if (Velocity.a < BColor.a + 0.04f)
        {
           cnt++;
           Out += BColor;
        }
     }

     Out /= (float)cnt;

     return Out;
}


//technique TShader
//{
//    // �ӵ� �� ���� 
//    pass P0
//    {
//        VertexShader = compile vs_1_1 VS_CreateVMap();
//        PixelShader = compile ps_2_0 PS_CreateVMap();
//    }
//
//    // �帲 ó�� 
//    pass P1
//    {
//        VertexShader = compile vs_1_1 VS_Final();
//        PixelShader = compile ps_3_0 PS_Final();
//    }
//}

#endif