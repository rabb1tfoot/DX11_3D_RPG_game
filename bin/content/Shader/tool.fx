#ifndef _TOOL
#define _TOOL

#include "value.fx"
#include "func.fx"

// ======================
// Grid Shader
// AlphaBlend : true
// g_int_0 : 간격 단계
// g_float_0 : 카메라 높이에 따른 그리드의 투명도
// g_float_1 : 선의 두께
// g_vec4_0  : 카메라 위치
// ======================

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
};

VTX_OUT VS_Grid(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV;

    return output;
}


float4 PS_Grid(VTX_OUT _in) : SV_Target
{
    // 현재 월드 위치값이 카메라로부터 멀면    
    float fDist = abs(distance(_in.vWorldPos, g_vec4_0.xyz));
    float fThickness = g_float_1 * (fDist / abs(g_vec4_0.y)) * 1.2f;

    // 현재보다 다음 단계 간격을 잡아낸다.
    int iStep = 1;
    for (int i = 0; i < g_int_0 + 1; ++i)
    {
        iStep *= 10;
    }

    if (fDist > 200.f && fDist > abs(g_vec4_0.y) * 5.f)
    {
        // 시야 멀어서 안보임
        clip(-1);
    }

    float fX = abs(_in.vWorldPos.x - (float) ((int) _in.vWorldPos.x / iStep) * iStep);
    float fZ = abs(_in.vWorldPos.z - (float) ((int) _in.vWorldPos.z / iStep) * iStep);

    if (fThickness / 2.f < fX)
        fX = (float) iStep - fX;
    if (fThickness / 2.f < fZ)
        fZ = (float) iStep - fZ;
    
    float fXRatio = 0.f;
    float fZRatio = 0.f;
    float fRatio = 0.f;

    if (fX < fThickness / 2.f || fZ < fThickness / 2.f)
    {
        fXRatio = ((fThickness / 2.f) - abs(fX)) / (fThickness / 2.f);
        fZRatio = ((fThickness / 2.f) - abs(fZ)) / (fThickness / 2.f);

        fRatio = 0.f;
        if (fXRatio > fZRatio)
            fRatio = fXRatio;
        else
            fRatio = fZRatio;

        return float4(1.f, 1.f, 1.f, fRatio);
    }
    
    // 현재 단계 라인
    iStep = 1;
    for (i = 0; i < g_int_0; ++i)
    {
        iStep *= 10;
    }

    fX = abs(_in.vWorldPos.x - (float) ((int) _in.vWorldPos.x / iStep) * iStep);
    fZ = abs(_in.vWorldPos.z - (float) ((int) _in.vWorldPos.z / iStep) * iStep);

    if (fThickness / 2.f < fX)
        fX = (float) iStep - fX;
    if (fThickness / 2.f < fZ)
        fZ = (float) iStep - fZ;
    
    fXRatio = 0.f;
    fZRatio = 0.f;
    fRatio = 0.f;

    if (fX < fThickness / 2.f || fZ < fThickness / 2.f)
    {
        fXRatio = ((fThickness / 2.f) - abs(fX)) / (fThickness / 2.f);
        fZRatio = ((fThickness / 2.f) - abs(fZ)) / (fThickness / 2.f);

        fRatio = 0.f;
        if (fXRatio > fZRatio)
            fRatio = fXRatio;
        else
            fRatio = fZRatio;

        return float4(1.f, 1.f, 1.f, fRatio * g_float_0);
    }
    
    clip(-1);
    return float4(0.f, 0.f, 0.f, 0.f);
}

#endif