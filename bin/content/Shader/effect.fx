#include "value.fx"

#ifndef _EFFECT
#define _EFFECT

// ========================
// 2DShadow Shader
// Alpablending
// 반드시 RectMesh 로 호출되어야 함
// ========================

struct VTX_INPUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUTPUT
{
    float4 vProjPos : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VTX_OUTPUT VS_2DShadow(VTX_INPUT _input)
{
    if (0.5f == _input.vLocalPos.y)
    {
        _input.vLocalPos.x += 0.3f;
        _input.vLocalPos.y -= 0.25f;
    }
    else
    {
        _input.vLocalPos.x -= 0.3f;
    }

    VTX_OUTPUT output = (VTX_OUTPUT) 0.f;
    output.vProjPos = mul(float4(_input.vLocalPos, 1.f), g_matWVP);
    output.vUV = _input.vUV;

    return output;
}

float4 PS_2DShadow(VTX_OUTPUT _input) : SV_Target
{
    float2 vAnimUV = vLT + (vSize * _input.vUV);
    float4 vColor = g_tex_anim.Sample(g_sam_0, vAnimUV);
    vColor.rgb = float3(0.1f, 0.1f, 0.1f);
    
    return vColor;
}

#endif