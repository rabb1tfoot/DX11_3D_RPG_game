#ifndef _TERRAIN
#define _TERRAIN

#include "value.fx"
#include "func.fx"

static float g_fMaxTess = 16.f;

struct VS_TERRAIN_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_TERRAIN_OUT
{
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
    float2 vFullUV : TEXCOORD1;
};

struct PS_TERRAIN_OUT
{
    float4 vDiffuse : SV_Target;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
};

// ====================
// Terrain Shader
// g_tex_0 : Weight
// g_tex_2 : Height Map
// g_tex_3 : Brush Tex

// g_arrtex_0 : tileTexArr
// g_arrtex_1 : normalTexArr

// g_int_0 : Terrain Face X
// g_int_1 : Terrain Face Z

// g_float_0 : Tess MinDist
// g_float_1 : Tess MaxDist
// g_float_2 : Tess Max Level

// g_vec4_0 : World Scale;
// g_vec4_1 : World MainCamera Pos
// ====================
VS_TERRAIN_OUT VS_Terrain(VS_TERRAIN_IN _in)
{
    VS_TERRAIN_OUT output = (VS_TERRAIN_OUT) 0.f;
    
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    output.vUV = _in.vUV;
    output.vFullUV = float2(_in.vUV.x / (float) g_int_0, _in.vUV.y / (float) g_int_1);
    
    return output;
}

// HullShader 
// 패치상수함수
struct TessLv
{
    float vEdgeFactor[3] : SV_TessFactor;
    float fInsideFactor : SV_InsideTessFactor;
};
    
TessLv TerrainPatchConstFunc(InputPatch<VS_TERRAIN_OUT, 3> _patch, uint _patchID : SV_PrimitiveID)
{
    TessLv output = (TessLv) 0.f;
    
    // 카메라와의 거리에 따라서 분할 레벨 지정
    float3 vCamPos = g_vec4_1.xyz;
    float fMaxDist = 6400.f;
    float fMinDist = 1000.f;
    float fMaxLv = 5.f;
    float3 vCenterPos = (_patch[0].vWorldPos + _patch[1].vWorldPos) / 2.f;
    
    float3 vSidePos = float3(vCenterPos.x - (vCenterPos.x - _patch[0].vWorldPos.x) * 2.f, vCenterPos.y, vCenterPos.z);
    float3 vFBPos = float3(vCenterPos.x, vCenterPos.y, vCenterPos.z - (vCenterPos.z - _patch[1].vWorldPos.z) * 2.f);
    
    float fLevel = CalTessLevel(vCamPos, vCenterPos, fMinDist, fMaxDist, fMaxLv);
    float fSideLevel = CalTessLevel(vCamPos, vSidePos, fMinDist, fMaxDist, fMaxLv);
    float fFBLevel = CalTessLevel(vCamPos, vFBPos, fMinDist, fMaxDist, fMaxLv);
    
    if (fFBLevel > fLevel)
        output.vEdgeFactor[0] = fFBLevel; // 밑, 윗
    else
        output.vEdgeFactor[0] = fLevel; // 밑, 윗       
    
    if (fSideLevel > fLevel)
        output.vEdgeFactor[1] = fSideLevel; // 좌, 우
    else
        output.vEdgeFactor[1] = fLevel; // 좌, 우
            
    output.vEdgeFactor[2] = fLevel; // 빗변
    output.fInsideFactor = fLevel;
    
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputcontrolpoints(3)]
[patchconstantfunc("TerrainPatchConstFunc")]
[maxtessfactor(64.f)]
[outputtopology("triangle_cw")]
VS_TERRAIN_OUT HS_Terrain(InputPatch<VS_TERRAIN_OUT, 3> _patch, uint i : SV_OutputControlPointID, uint _patchID : SV_PrimitiveID)
{
    VS_TERRAIN_OUT output = (VS_TERRAIN_OUT) 0.f;
    
    output.vWorldPos = _patch[i].vWorldPos;
    
    output.vUV = _patch[i].vUV;
    output.vFullUV = _patch[i].vFullUV;
    
    return output;
}

struct DS_TERRAIN_OUT
{
    float4 vPosition : SV_Position;
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    float2 vUV : TEXCOORD;
    float2 vFullUV : TEXCOORD1;
};


[domain("tri")]
DS_TERRAIN_OUT DS_Terrain(OutputPatch<VS_TERRAIN_OUT, 3> _patch, float3 _vRatio : SV_DomainLocation, TessLv _tess)
{
    DS_TERRAIN_OUT output = (DS_TERRAIN_OUT) 0.f;
    
    output.vUV = _patch[0].vUV * _vRatio[0]
                        + _patch[1].vUV * _vRatio[1]
                        + _patch[2].vUV * _vRatio[2];
    
    
    output.vFullUV = _patch[0].vFullUV * _vRatio[0]
                        + _patch[1].vFullUV * _vRatio[1]
                        + _patch[2].vFullUV * _vRatio[2];
    
    float3 vWorldPos = _patch[0].vWorldPos * _vRatio[0]
                        + _patch[1].vWorldPos * _vRatio[1]
                        + _patch[2].vWorldPos * _vRatio[2];
    
    vWorldPos.y = g_tex_2.SampleLevel(g_sam_1, output.vFullUV, 0).x * 1000.f /*g_vec2_0.y*/;
    
    output.vPosition = mul(mul(float4(vWorldPos, 1.f), g_matView), g_matProj);
    output.vViewPos = mul(float4(vWorldPos, 1.f), g_matView).xyz;
    
    // Texcell UV Step
    float2 vUVStep = float2(1.f / ((float) g_int_0 * g_fMaxTess), 1.f / ((float) g_int_1 * g_fMaxTess));
    
    float3 vRightPos = vWorldPos;
    vRightPos.x += (1.f / g_fMaxTess) * 100.f; /*g_vec2_0.x*/
    
    float3 vLeftPos = vWorldPos;
    vLeftPos.x -= (1.f / g_fMaxTess) * 100.f;
    
    float3 vUpPos = vWorldPos;
    vUpPos.z += (1.f / g_fMaxTess) * 100.f;
    
    float3 vDownPos = vWorldPos;
    vDownPos.z -= (1.f / g_fMaxTess) * 100.f;
    
    float2 vNearUV = output.vFullUV;
    vNearUV.x += vUVStep.x;
    vRightPos.y = g_tex_2.SampleLevel(g_sam_1, vNearUV, 0).x * 1000.f; /*g_vec2_0.y*/
    
    vNearUV = output.vFullUV;
    vNearUV.x -= vUVStep.x;
    vLeftPos.y = g_tex_2.SampleLevel(g_sam_1, vNearUV, 0).x * 1000.f; /*g_vec2_0.y*/
   
    vNearUV = float2(output.vFullUV.x, output.vFullUV.y - vUVStep.y);
    vUpPos.y = g_tex_2.SampleLevel(g_sam_1, vNearUV, 0).x * 1000.f; /*g_vec2_0.y*/
    
    vNearUV = float2(output.vFullUV.x, output.vFullUV.y + vUVStep.y);
    vDownPos.y = g_tex_2.SampleLevel(g_sam_1, vNearUV, 0).x * 1000.f; /*g_vec2_0.y*/
    
    float3 vTangent = normalize(vRightPos - vLeftPos);
    float3 vBinormal = normalize(vUpPos - vDownPos);
    
    output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matView));
    output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matView));
    output.vViewNormal = normalize(cross(output.vViewBinormal, output.vViewTangent));
        
    return output;
}


PS_TERRAIN_OUT PS_Terrain(DS_TERRAIN_OUT _in)
{
    PS_TERRAIN_OUT output = (PS_TERRAIN_OUT) 0.f;
    
    // 지형 색상(타일에서 추출)

    float4 vWeight = g_tex_0.Sample(g_sam_0, _in.vFullUV);

    if (g_arrtcheck_0)
    {
        for (int i = 0; i < 4; ++i)
        {
            output.vDiffuse += g_arrtex_0.Sample(g_sam_0, float3(_in.vUV, float(i))) * vWeight[i];
        }
    }
    else
        output.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);

    //노말값 추출
    if(g_arrtcheck_1)
    {
        float3 vViewNormal = (float3) 0.f;
        for (int i = 0; i < 4; ++i)
        {
            vViewNormal += g_arrtex_1.Sample(g_sam_0, float3(_in.vUV, float(i))).xyz * vWeight[i];

        }
        float3x3 matTBN = { _in.vViewTangent, _in.vViewBinormal, _in.vViewNormal };
        vViewNormal = (vViewNormal - 0.5f) * 2.f;
        vViewNormal = mul(vViewNormal.xyz, matTBN);
        output.vNormal.xyz = vViewNormal;
    }
    else
        output.vNormal.xyz = _in.vViewNormal.xyz;

    
    // 브러쉬 택스처가 세팅되었다면 포시젼타깃w에 기록
    if(g_tcheck_3)
    {
        float2 vBrushLTUV = g_vec2_0 - (g_vec2_1 / 2.f);
        float2 vDiffUV = _in.vFullUV - vBrushLTUV;
        float2 vBrushUV = vDiffUV / g_vec2_1;

        // 해당 브러쉬 범위에만 추출해서 기록
        if(0 <= vBrushUV.x && vBrushUV.x <= 1.f
            && 0 <= vBrushUV.y && vBrushUV.y <= 1.f)
        {
            output.vPosition.w = g_tex_3.Sample(g_sam_0, vBrushUV).a;
        }
    }
    output.vPosition.xyz = _in.vViewPos;
    
    return output;
}


// ================
// Tesselation Test
// ================
struct VS_TESS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_TESS_OUT
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD;
};

VS_TESS_OUT VS_Tess(VS_TESS_IN _in)
{
    VS_TESS_OUT output = (VS_TESS_OUT) 0.f;
    
    output.vPosition = _in.vPos;
    output.vUV = _in.vUV;
    
    return output;
}


struct HS_OUT
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD;
};

// HullShader 
// 패치상수함수

    
TessLv PatchConstFunc(InputPatch<VS_TESS_OUT, 3> _patch, uint _patchID : SV_PrimitiveID)
{
    TessLv output = (TessLv) 0.f;
    
    output.vEdgeFactor[0] = 16;
    output.vEdgeFactor[1] = 16;
    output.vEdgeFactor[2] = 16;
    output.fInsideFactor = 16.f;
    
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstFunc")]
[maxtessfactor(64.f)]
[outputtopology("triangle_cw")]
HS_OUT HS_Tess(InputPatch<VS_TESS_OUT, 3> _patch, uint i : SV_OutputControlPointID, uint _patchID : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vPosition = _patch[i].vPosition;
    output.vUV = _patch[i].vUV;
    
    return output;
}

// Domain Shader
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

[domain("tri")]
DS_OUT DS_Tess(OutputPatch<VS_TESS_OUT, 3> _patch, float3 _vRatio : SV_DomainLocation, TessLv _tess)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    float3 vLocalPos = _patch[0].vPosition * _vRatio[0]
                        + _patch[1].vPosition * _vRatio[1]
                        + _patch[2].vPosition * _vRatio[2];
    
    float2 vUV = _patch[0].vUV * _vRatio[0]
                        + _patch[1].vUV * _vRatio[1]
                        + _patch[2].vUV * _vRatio[2];
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
    
    return output;
}

float4 PS_Tess(DS_OUT _in) : SV_Target
{
    float4 vColor = (float4) float4(1.f, 0.f, 1.f, 1.f);
    
    return vColor;
}






#endif