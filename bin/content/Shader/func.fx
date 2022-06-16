#ifndef _FUNC
#define _FUNC

#include "value.fx"

void CaculateLight(float3 _vViewPos, float3 _vViewNormal, int _iLightIdx, inout tLightColor _color)
{
    float fPow = 0.f;
    float fReflectPow = 0.f;
    float3 vLightViewDir = (float3) 0.f;
    float3 vViewReflectDir = (float3) 0.f;
           
    // Directional Light
    if (0 == g_arrLight3D[_iLightIdx].iLightType)
    {
        vLightViewDir = mul(float4(g_arrLight3D[_iLightIdx].vLightDir.xyz, 0.f), g_matView).xyz;
        fPow = saturate(dot(-vLightViewDir, _vViewNormal));

        // 광원에서 오는 방향 벡터를 노발 벡터 쪽으로 투영시킨 길이
        vViewReflectDir = normalize(vLightViewDir + 2 * dot(-vLightViewDir, _vViewNormal) * _vViewNormal);

        // 시선벡터
        float3 vViewEye = normalize(_vViewPos);
        fReflectPow = saturate(dot(-vViewEye, vViewReflectDir));
        fReflectPow = pow(fReflectPow, 10);
    }
    //POINT LIGHT
    else if (1 == g_arrLight3D[_iLightIdx].iLightType)
    {
        // View Space 에서 광원 위치
        float3 vLightViewPos = mul(float4(g_arrLight3D[_iLightIdx].vLightWorldPos.xyz, 1.f), g_matView).xyz;

        // View Space 에서 광원으로 부터 오는 방향
        vLightViewDir = _vViewPos - vLightViewPos;
        float fDistance = length(vLightViewDir);
        vLightViewDir = normalize(vLightViewDir);
       
        // 거리에 따른 비율을 계산
        float fRatio = (g_arrLight3D[_iLightIdx].fLightRange - fDistance) / g_arrLight3D[_iLightIdx].fLightRange;
        fPow = saturate(dot(-vLightViewDir, _vViewNormal));
        fPow *= fRatio;
               
        // 광원에서 오는 방향 벡터를 노발 벡터 쪽으로 투영시킨 길이
        vViewReflectDir = normalize(vLightViewDir + 2 * dot(-vLightViewDir, _vViewNormal) * _vViewNormal);

        // 시선벡터
        float3 vViewEye = normalize(_vViewPos);
        fReflectPow = saturate(dot(-vViewEye, vViewReflectDir));
        fReflectPow = pow(fReflectPow, 10);
    }
    //SPOT LIGHT
    else if (2 == g_arrLight3D[_iLightIdx].iLightType)
    {
         // View Space 에서 광원 위치
        float3 vLightViewPos = mul(float4(g_arrLight3D[_iLightIdx].vLightWorldPos.xyz, 1.f), g_matView);

        //View Space에서 광원이 바라보는 방향
        float3 vLightOriginViewDir = mul(float4(g_arrLight3D[_iLightIdx].vLightDir.xyz, 0.f), g_matView);

        // View Space 에서 광원으로 부터 오는 방향
        vLightViewDir = _vViewPos - vLightViewPos;
        float fDistance = length(vLightViewDir);
        vLightViewDir = normalize(vLightViewDir);
       
        // 거리에 따른 비율을 계산
        float fRatio = (g_arrLight3D[_iLightIdx].fLightRange - fDistance) / g_arrLight3D[_iLightIdx].fLightRange;
        fPow = saturate(dot(-vLightViewDir, _vViewNormal));
        fPow *= fRatio;
               
        // 광원에서 오는 방향 벡터를 노발 벡터 쪽으로 투영시킨 길이
        vViewReflectDir = normalize(vLightViewDir + 2 * dot(-vLightViewDir, _vViewNormal) * _vViewNormal);

        // 시선벡터
        float3 vViewEye = normalize(_vViewPos);
        fReflectPow = saturate(dot(-vViewEye, vViewReflectDir));
        fReflectPow = pow(fReflectPow, 10);

        //각도에 따른 컬링 계산
        if (dot(vLightOriginViewDir, vLightViewDir) - cos(g_arrLight3D[_iLightIdx].fLightAngle) <= 0)
        {
            fPow = 0.f;
            fReflectPow = 0.f;
        }
    }
    
    _color.vLightDiffCol += g_arrLight3D[_iLightIdx].tColor.vLightDiffCol * fPow;
    _color.vLightSpec += g_arrLight3D[_iLightIdx].tColor.vLightSpec * fReflectPow;
    _color.vLightAmb += g_arrLight3D[_iLightIdx].tColor.vLightAmb;
}

float CalTessLevel(in float3 _vWorldCamPos, float3 _vPatchPos, float _fMin, float _fMax, float _fMaxLv)
{
    float fLen = length(_vPatchPos - _vWorldCamPos);
            
    float fLevel = (_fMaxLv - 1.f) * ((1.f - saturate((fLen - _fMin) / (_fMax - _fMin))));
    if (fLevel == _fMaxLv - 1.f)
    {
        fLevel += 1.f;
    }
        
    fLevel = ceil(fLevel);
    fLevel = pow(2, fLevel);
    
    return fLevel;
}

int IntersectsLay(float3 _vertices[3], float3 _vStart, float3 _vDir, out float _fResult)
{
    float3 edge[2];
    edge[0] = _vertices[1].xyz - _vertices[0].xyz;
    edge[1] = _vertices[2].xyz - _vertices[0].xyz;

    float3 normal = normalize(cross(edge[0], edge[1]));
    float b = dot(normal, _vDir);

    float3 w0 = _vStart - _vertices[0].xyz;
    float a = -dot(normal, w0);
    float t = a / b;

    _fResult = t;

    float3 p = _vStart + t * _vDir;
    float uu, uv, vv, wu, wv, inverseD;
    uu = dot(edge[0], edge[0]);
    uv = dot(edge[0], edge[1]);
    vv = dot(edge[1], edge[1]);

    float3 w = p - _vertices[0].xyz;
    wu = dot(w, edge[0]);
    wv = dot(w, edge[1]);
    inverseD = uv * uv - uu * vv;
    inverseD = 1.0f / inverseD;

    float u = (uv * wv - vv * wu) * inverseD;
    if (u < 0.0f || u > 1.0f)
    {
        return 0;
    }

    float v = (uv * wu - uu * wv) * inverseD;
    if (v < 0.0f || (u + v) > 1.0f)
    {
        return 0;
    }
        
    return 1;
}


float3 GetIntercestsPos(float3 _vertices[3], float3 _vStart, float3 _vDir)
{
    // 평면의 방정식
    float3 vFlow1 = _vertices[1] - _vertices[0];
    float3 vFlow2 = _vertices[2] - _vertices[0];

    // 평면의 법선벡터
    float3 vNormal = normalize(cross(vFlow1, vFlow2));
    float fD = -dot(_vertices[0], vNormal);

    // 직선의 방정식
    float fT = (-dot(vNormal, _vStart) - fD) / dot(_vDir, vNormal);
    
    // 교점
    return float3(_vDir.x * fT + _vStart.x, _vDir.y * fT + _vStart.y, _vDir.z * fT + _vStart.z);
}

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
    matrix matBone =
    {
        g_tex_7.Load(int3(_iBoneIdx * 4, _iRowIdx, 0))
        , g_tex_7.Load(int3(_iBoneIdx * 4 + 1, _iRowIdx, 0))
        , g_tex_7.Load(int3(_iBoneIdx * 4 + 2, _iRowIdx, 0))
        , g_tex_7.Load(int3(_iBoneIdx * 4 + 3, _iRowIdx, 0))

    };
    return matBone;

}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
                           , inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }
    
    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}

void Skinning(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }
    
    _vPos = info.vPos;
}


static float gaussian5x5[25] =
{
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
};

static float gaussian3x3[9] =
{
    0.035f, 0.16f, 0.035f,
    0.16f, 0.22f, 0.16f,
    0.035f, 0.16f, 0.035f,
};

float4 gaussian3x3Sample(in int2 _uv, in RWTexture2D<float4> _tex)
{
    float4 fOut = (float4) 0.f;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            fOut += _tex[_uv + int2(i - 1, j - 1)] * gaussian3x3[i * 3 + j];
        }
    }
    return fOut;
}


float gaussian5x5Sample(in int2 _uv, in Texture2D _tex)
{
    float4 fOut = (float4) 0.f;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            fOut += _tex[_uv + int2(i - 2, j - 2)] * gaussian5x5[i * 5 + j];
        }
    }
    return fOut.x;
}

#endif