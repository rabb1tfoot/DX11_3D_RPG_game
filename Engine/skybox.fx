#ifndef _SKYBOX
#define _SKYBOX

#include "value.fx"

//========================
// Skybox Shader
// Rasteraizer : CULL_FRONT
// mesh : Sphere
// Depth func : LESS_QUAL
// g_tex_0 : Skybox Texture
//=========================

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VTX_OUT VS_Skybox(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    // View, Proj    
    float4 vViewPos = mul(float4(_in.vPos, 0.f), g_matView); // View 변환에서 회전부분만 영향
    float4 vProjPos = mul(vViewPos, g_matProj);
    
    //vProjPos.x; // Proj.x * View.z()
    //vProjPos.y; // Proj.y * View.z()
    //vProjPos.z; // View.z()
    //vProjPos.w; // View.z()
    
    vProjPos.z = vProjPos.w;
    
    output.vPosition = vProjPos;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Skybox(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    return vOutColor;
}
#endif