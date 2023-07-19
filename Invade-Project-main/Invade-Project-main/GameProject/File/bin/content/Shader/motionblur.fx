
#include "value.fx"
#include "func.fx"

#ifndef _MOTIONBLUR
#define _MOTIONBLUR

float4x4 m_WVPNew;             // 현재 월드 공간의 월드 x 뷰 x 투영
float4x4 m_WVPOld;             // 과거 월드 공간의 월드 x 뷰 x 사영 
float4x4 m_ROnly;               // 월드 공간에서 법선 벡터를 변환하기 위한 행렬. 법선 벡터이므로 회전 성분만. 

sampler tex0 : register(s0);   // 장면의 렌더링 이미지. Pass1에서 사용한다. 
sampler tex1 : register(s1);   // 속도 맵. Pass1에서 사용한다. 

// 속도 맵 생성 
struct VS_CreateVMap_OUTPUT
{
    float4 Pos      : POSITION;
    float4 Dir      : TEXCOORD0;   // 속도 벡터 
};

VS_CreateVMap_OUTPUT VS_CreateVMap(float4 Pos     : POSITION,
                                    float4 Normal : NORMAL,
                                    float2 Tex : TEXCOORD0)
{
    VS_CreateVMap_OUTPUT Out;

    // 법선 벡터는 회전 성분만의 행렬을 적응 
    float3 N = mul(Normal.xyz, m_ROnly);

    // 현재 정점의 좌표 
    float4 NewPos = mul(Pos, m_WVPNew);

    // 이전 좌표
    float4 OldPos = mul(Pos, m_WVPOld);

    // 정점의 이동 방향 벡터 
    float3 Dir = NewPos.xyz - OldPos.xyz;

    //정점의 이동 방향 벡터와 정점의 법선의 내적을 계산 
    float a = dot(normalize(Dir), normalize(N));

    //과거 방향으로 모델을 늘리는 
    if (a < 0.0f)
        Out.Pos = OldPos;
    else
        Out.Pos = NewPos;

    // 속도 벡터 계산 
    //-1.0 f 에서 1.0f 의 범위를 텍스처 좌표계의 0.0f ~ 1.0f 의 텍셀 위치를 참조하기 위해 거리를 반으로 한다
    Out.Dir.xy = (NewPos.xy / NewPos.w - OldPos.xy / OldPos.w) * 0.5f;

    // 최종적으로 텍셀의 오프셋 값이 되기 때문에 Y 방향을 역방향으로 한다
    Out.Dir.y *= -1.0f;

    // 장면의 Z 값을 계산하기 위한 매개 변수 
    Out.Dir.z = Out.Pos.z;
    Out.Dir.w = Out.Pos.w;

    return Out;
}

float4 PS_CreateVMap(VS_CreateVMap_OUTPUT In) : COLOR0
{
   float4 Out;

    // 속도 벡터 
    Out.xy = In.Dir.xy;

    // 미사용 
    Out.z = 1.0f;

    // Z 값 계산 
    Out.w = In.Dir.z / In.Dir.w;

    return Out;
}

// 속도 맵 을 참조하여 흐림 처리
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

    // 보케의 매끄러움. 수치를 크게 하면 부드러워진다. 
    int Blur = 10;

    // 속도 맵에서 속도 벡터와 Z 값을 가져옵니다. 
    float4 Velocity = tex2D(tex1, In.Tex);

    Velocity.xy /= (float)Blur;

    int cnt = 1;
    float4 BColor;

    // 장면 렌더링 이미지를 가져옵니다. a 성분에 Z 값이 저장된다. 
    float4 Out = tex2D(tex0, In.Tex);

    for (int i = cnt; i < Blur; i++)
    {
        // 속도 벡터 방향의 텍셀 위치를 참조하여 장면 렌더링 이미지의 색 정보를 가져옵니다.
        BColor = tex2D(tex0, In.Tex + Velocity.xy * (float)i);

        // 속도 맵의 Z값과 속도 벡터 방향에 있는 텍셀 위치를 샘플링한 장면의 렌더링 이미지의 Z값을 비교한다. (주의 1) 
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
//    // 속도 맵 생성 
//    pass P0
//    {
//        VertexShader = compile vs_1_1 VS_CreateVMap();
//        PixelShader = compile ps_2_0 PS_CreateVMap();
//    }
//
//    // 흐림 처리 
//    pass P1
//    {
//        VertexShader = compile vs_1_1 VS_Final();
//        PixelShader = compile ps_3_0 PS_Final();
//    }
//}

#endif