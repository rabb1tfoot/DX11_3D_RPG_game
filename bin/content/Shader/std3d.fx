#ifndef _STD3D
#define _STD3D

#include "value.fx"
#include "func.fx"

struct VTX_IN
{
    float3 vPos : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vLightPow : POSITION;
};




// =====================
// 3D Phong Shader 
// g_tex_0 : 출력 텍스쳐
// g_tex_1 : 노말 텍스쳐
// =====================
struct VTX_OUT_PHONG
{
    float4 vPosition : SV_Position;
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    float2 vUV : TEXCOORD;
};

struct PS_OUT_PHONG
{
    float4 vDiffuse : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
};

VTX_OUT_PHONG VS_Phong(VTX_IN _in)
{
    VTX_OUT_PHONG output = (VTX_OUT_PHONG) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV));
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV));
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV));

    output.vUV = _in.vUV;

    return output;
}

PS_OUT_PHONG PS_Phong(VTX_OUT_PHONG _in)
{
    PS_OUT_PHONG output = (PS_OUT_PHONG) 0.f;

    output.vDiffuse.xyz = g_tex_0.Sample(g_sam_0, _in.vUV);

    float3x3 matTBN =
    {
        _in.vViewTangent,
        _in.vViewBinormal,
        _in.vViewNormal
    };

    // 0 ~ 1 사이의 값을 -1 ~ 1 사이로 확장한다.
    float4 vTexNormal = g_tex_1.Sample(g_sam_0, _in.vUV);
    vTexNormal.xyz = (vTexNormal.xyz - 0.5f) * 2.f;
    float3 vViewNormal = mul(vTexNormal.xyz, matTBN);

    output.vNormal.xyz = vViewNormal;
    output.vPosition.xyz = _in.vViewPos;

       
    //tLightColor tCol = (tLightColor) 0.f;

    //for (int i = 0; i < g_iLight3DCount; ++i)
    //{
    //    CaculateLight(_in.vViewPos, vViewNormal, i, tCol);
    //    //CaculateLight(_in.vViewPos, _in.vViewNormal, i, tCol);
    //}     

    //vOutColor.rgb = (tCol.vLightDiff.rgb * vOutColor.rgb) + (tCol.vLightSpec.rgb) + (vOutColor.rgb * tCol.vLightAmb.rgb);

    return output;
}


// ==============================
// STD 3D Shader
// g_tex_0 : 출력 텍스쳐
// g_tex_1 : 노말 텍스쳐
// g_tex_2 : 반사광 텍스쳐
// g_tex_3 : 발광(Emissive) 텍스쳐
// ==============================

struct VTX_STD3D_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD;

    float4 vWeights : WEIGHTS;
    float4 vIndices : BLENDINDICES;

    //instancing
    row_major matrix g_matWorld :WORLD;
    row_major matrix g_matWV : WV;
    row_major matrix g_matWVP : WVP;
    int iRowIdx : ROWINDEX; //애니메이션

    uint iInstanceID : SV_InstanceID;
};

struct VTX_STD3D_OUT
{
    float4 vPosition : SV_Position;
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    float2 vUV : TEXCOORD;
};

struct PS_STD3D_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
};


VTX_STD3D_OUT VS_STD3D(VTX_STD3D_IN _in)
{
    VTX_STD3D_OUT output = (VTX_STD3D_OUT) 0.f;

     // 8 번째 텍스쳐는 Bone Matrix Tex(VTF - vertex texture fetch) 예약자리
    if (g_tcheck_7)
    {
        Skinning(_in.vPosition, _in.vTangent
        , _in.vBinormal, _in.vNormal
        , _in.vWeights, _in.vIndices, 0);
    }

    output.vPosition = mul(float4(_in.vPosition, 1.f), g_matWVP);

    output.vViewPos = mul(float4(_in.vPosition, 1.f), g_matWV);
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV));
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV));
    output.vViewNormal = normalize(cross(output.vViewBinormal, output.vViewTangent));

    output.vUV = _in.vUV;

    return output;
}


VTX_STD3D_OUT VS_STD3D_Inst(VTX_STD3D_IN _in)
{
    VTX_STD3D_OUT output = (VTX_STD3D_OUT) 0.f;

     // 8 번째 텍스쳐는 Bone Matrix Tex(VTF - vertex texture fetch) 예약자리
    if (g_tcheck_7)
    {
        Skinning(_in.vPosition, _in.vTangent
        , _in.vBinormal, _in.vNormal
        , _in.vWeights, _in.vIndices, _in.iRowIdx);
    }

    output.vPosition = mul(float4(_in.vPosition, 1.f), _in.g_matWVP);

    output.vViewPos = mul(float4(_in.vPosition, 1.f), _in.g_matWV);
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), _in.g_matWV));
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), _in.g_matWV));
    output.vViewNormal = normalize(cross(output.vViewBinormal, output.vViewTangent));

    output.vUV = _in.vUV;

    return output;
}

PS_STD3D_OUT PS_STD3D(VTX_STD3D_OUT _in)
{
    PS_STD3D_OUT output = (PS_STD3D_OUT) 0.f;

    if (g_tcheck_0)
    {
        output.vDiffuse.xyz = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    else
    {
        output.vDiffuse.xyz = float3(1.f, 0.f, 1.f);
    }
        
    if (g_tcheck_1)
    {
        float3x3 matTBN =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal
        };
        
        // 0 ~ 1 사이의 값을 -1 ~ 1 사이로 확장한다.
        float4 vTexNormal = g_tex_1.Sample(g_sam_0, _in.vUV);
        vTexNormal.xyz = (vTexNormal.xyz - 0.5f) * 2.f;
        float3 vViewNormal = float3(0.f, 0.f, 0.f);

        //정점에 노말값이 없고 텍스처만 있는경우 노말텍스처만 사용
       if (g_int_0)
        {
            output.vNormal.xyz = normalize(mul(float4(vTexNormal.xyz, 0.f), g_matWV)).xyz;
        }
       else 
       {
            output.vNormal.xyz = mul(vTexNormal.xyz, matTBN);
       }
    }
    else
    {
        output.vNormal.xyz = _in.vViewNormal;
    }
   
    output.vPosition.xyz = _in.vViewPos;

    return output;
}

// ==========================
// Collider3D Shader
// g_int_0 : Collision Check
// ==========================

struct COLL_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD;

    float4 vWeights : WEIGHTS;
    float4 vIndices : BLENDINDICES;

    //instancing
    row_major matrix g_matWorld : WORLD;
    row_major matrix g_matWV : WV;
    row_major matrix g_matWVP : WVP;
    int iRowIdx : ROWINDEX; //애니메이션

    uint iInstanceID : SV_InstanceID;
};

struct COLL_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

COLL_OUT VS_Collider3D(COLL_IN _input)
{
    COLL_OUT output = (COLL_OUT) 0.f;

    float4 vWorldPos = mul(float4(_input.vPosition, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);

    output.vPosition = mul(vViewPos, g_matProj);
    output.vUV = _input.vUV;

    return output;
}

COLL_OUT VS_Collider3D_Inst(COLL_IN _input)
{
    COLL_OUT output = (COLL_OUT) 0.f;

    output.vPosition = mul(float4(_input.vPosition, 1.f), _input.g_matWVP);
    output.vUV = _input.vUV;

    return output;
}

float4 PS_Collider3D(COLL_OUT _input) : SV_Target
{
    float4 vColor = (float4)0.f;

    if (g_int_0)
        vColor = float4(1.f, 0.2f, 0.2f, 1.f);
    else
        vColor = float4(0.2f, 1.f, 0.2f, 1.f);

    return vColor;
}

#endif
