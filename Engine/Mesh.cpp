#include "stdafx.h"
#include "Mesh.h"

#include "Device.h"
#include "Shader.h"

#include "ResMgr.h"
#include "FBXLoader.h"
#include "instancingBuffer.h"

CMesh::CMesh()
	: m_pVB(NULL)
	, m_pLayout(NULL)
	, m_iOffset(0)
	, m_iOffsetInst(0)
	, m_iVtxSize(0)
	, m_iVtxCount(0)
	, m_pVtxMem(nullptr)
	, CResource(RES_TYPE::MESH)
{
}

CMesh::~CMesh()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pLayout);

	SAFE_DELETE(m_pVtxMem);

	for (UINT i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		SAFE_DELETE(m_vecIdxInfo[i].pIdxSysMem);
		SAFE_RELEASE(m_vecIdxInfo[i].pIB);
	}
}

void CMesh::CreateMesh(UINT _iVtxSize, UINT _iVtxCount, D3D11_USAGE _eVtxUsage, void* _pVtx,
	UINT _iIdxCount, D3D11_USAGE _eIdxUsage, DXGI_FORMAT _eIdxFormat, void* _pIdx)
{
	// Vertex buffer 생성	
	m_tVtxDesc = {};
	m_tVtxDesc.ByteWidth = _iVtxSize * _iVtxCount;
	m_tVtxDesc.Usage = _eVtxUsage;
	if (D3D11_USAGE::D3D11_USAGE_DYNAMIC == m_tVtxDesc.Usage)
	{
		m_tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	m_tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = _pVtx;
	m_pVtxMem = malloc(_iVtxSize * _iVtxCount);
	for (UINT i = 0; i < _iVtxCount; ++i)
	{
		((VTX*)m_pVtxMem)[i].vPos = ((VTX*)_pVtx)[i].vPos;
		((VTX*)m_pVtxMem)[i].vColor = ((VTX*)_pVtx)[i].vColor;
		((VTX*)m_pVtxMem)[i].vUV = ((VTX*)_pVtx)[i].vUV;
		((VTX*)m_pVtxMem)[i].vNormal = ((VTX*)_pVtx)[i].vNormal;
		((VTX*)m_pVtxMem)[i].vTangent = ((VTX*)_pVtx)[i].vTangent;
		((VTX*)m_pVtxMem)[i].vBinormal = ((VTX*)_pVtx)[i].vBinormal;
		((VTX*)m_pVtxMem)[i].vWeights = ((VTX*)_pVtx)[i].vWeights;
		((VTX*)m_pVtxMem)[i].vIndices = ((VTX*)_pVtx)[i].vIndices;
	}

	if (FAILED(DEVICE->CreateBuffer(&m_tVtxDesc, &tSubData, &m_pVB)))
	{
		assert(nullptr);
	}

	// Index Buffer 생성	
	D3D11_BUFFER_DESC tIdxDesc = {};
	tIdxDesc.ByteWidth = GetSizeofFormat(_eIdxFormat) * _iIdxCount;
	tIdxDesc.Usage = _eIdxUsage;
	if (D3D11_USAGE::D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
	{
		tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	tSubData = {};
	tSubData.pSysMem = _pIdx;

	ID3D11Buffer* pIdxBuff = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSubData, &pIdxBuff)))
	{
		assert(nullptr);
	}

	m_iVtxSize = _iVtxSize;
	m_iVtxCount = _iVtxCount;

	tIndexInfo tIdxInfo = {};

	tIdxInfo.eFormat = _eIdxFormat;
	tIdxInfo.pIB = pIdxBuff;
	tIdxInfo.iIdxCount = _iIdxCount;
	tIdxInfo.tIdxDesc = tIdxDesc;
	tIdxInfo.pIdxSysMem = malloc(sizeof(UINT) * _iIdxCount);
	memcpy(tIdxInfo.pIdxSysMem, _pIdx, sizeof(UINT) * _iIdxCount);

	m_vecIdxInfo.push_back(tIdxInfo);

	// Layout 정보 만들기	
	AddInputLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	AddInputLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	AddInputLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);

	AddInputLayout("WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	AddInputLayout("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	/*인스턴싱 데이터는 슬롯이 다르다*/
	AddInputLayout("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("ROWINDEX", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
}

void CMesh::CreateMesh(UINT _iVtxCount, UINT _iVtxSize, D3D11_USAGE _eVtxUsage, void * _pVtxSysMem
	, vector<tIndexInfo>& _IdxInfo)
{
	m_tVtxDesc.ByteWidth = _iVtxSize * _iVtxCount;
	m_tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	m_tVtxDesc.Usage = _eVtxUsage;
	if (D3D11_USAGE_DYNAMIC == m_tVtxDesc.Usage)
		m_tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _pVtxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&m_tVtxDesc, &tSub, &m_pVB)))
	{
		assert(nullptr);
	}

	for (UINT i = 0; i < _IdxInfo.size(); ++i)
	{
		tIndexInfo tIdxInfo = _IdxInfo[i];
		tSub.pSysMem = tIdxInfo.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&tIdxInfo.tIdxDesc, &tSub, &tIdxInfo.pIB)))
		{
			assert(nullptr);
		}
		m_vecIdxInfo.push_back(tIdxInfo);
	}

	m_pVtxMem = _pVtxSysMem;
	m_iVtxCount = _iVtxCount;
	m_iVtxSize = _iVtxSize;
	//pMesh->SetVtxSysMem(_pVtxSysMem, _iVtxSize, _iVtxCount);

	for (UINT i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		//pMesh->SetIdxSysMem(vecInfo[i].pIdxSysMem, vecInfo[i].iIdxSize, vecInfo[i].iIdxCount, i);
	}

	// Layout 정보 만들기	
	AddInputLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	AddInputLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	AddInputLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);

	AddInputLayout("WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	AddInputLayout("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	/*인스턴싱 데이터는 슬롯이 다르다*/
	AddInputLayout("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("ROWINDEX", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
}


CMesh * CMesh::CreateFromContainer(CFBXLoader & _loader)
{
	const tContainer* container = &_loader.GetContainer(0);

	UINT iVtxCount = container->vecPos.size();

	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = sizeof(VTX) * iVtxCount;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = malloc(tBufferDesc.ByteWidth);
	VTX* pSys = (VTX*)tSub.pSysMem;
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		pSys[i].vPos = container->vecPos[i];
		pSys[i].vUV = container->vecUV[i];
		pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
		pSys[i].vNormal = container->vecNormal[i];
		pSys[i].vTangent = container->vecTangent[i];
		pSys[i].vBinormal = container->vecBinormal[i];
		pSys[i].vWeights = container->vecWeights[i];
		pSys[i].vIndices = container->vecIndices[i];
	}

	ID3D11Buffer* pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pVB)))
	{
		return NULL;
	}

	CMesh* pMesh = new CMesh;
	pMesh->m_pVB = pVB;
	pMesh->m_iVtxCount = iVtxCount;
	pMesh->m_iVtxSize = sizeof(VTX);
	pMesh->m_tVtxDesc = tBufferDesc;
	pMesh->m_pVtxMem = pSys;

	// 인덱스 정보
	UINT iIdxBufferCount = container->vecIdx.size();

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIndexInfo info;
		info.iIdxCount = container->vecIdx[i].size();
		info.eFormat = DXGI_FORMAT_R32_UINT;

		memset(&tBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		tBufferDesc.ByteWidth = container->vecIdx[i].size() * GetSizeofFormat(DXGI_FORMAT_R32_UINT);

		tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
			tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		info.pIdxSysMem = malloc(tBufferDesc.ByteWidth);
		memcpy(info.pIdxSysMem, &container->vecIdx[i][0], tBufferDesc.ByteWidth);
		tSub.pSysMem = info.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &info.pIB)))
		{
			return NULL;
		}

		info.tIdxDesc = tBufferDesc;
		pMesh->m_vecIdxInfo.push_back(info);
	}

	// Layout 정보 넣기
	pMesh->AddInputLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddInputLayout("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddInputLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	pMesh->AddInputLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddInputLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddInputLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);

	pMesh->AddInputLayout("WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddInputLayout("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	/*인스턴싱 데이터는 슬롯이 다르다*/
	pMesh->AddInputLayout("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddInputLayout("WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddInputLayout("WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddInputLayout("WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddInputLayout("ROWINDEX", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	// Animation3D
	if (!container->bAnimation)
		return pMesh;

	vector<tBone*>& vecBone = _loader.GetBones();

	for (UINT i = 0; i < vecBone.size(); ++i)
	{
		tMTBone bone = {};
		bone.iDepth = vecBone[i]->iDepth;
		bone.iParentIndx = vecBone[i]->iParentIndx;
		bone.matBone = GetMatrix(vecBone[i]->matBone);
		bone.matOffset = GetMatrix(vecBone[i]->matOffset);
		bone.strBoneName = vecBone[i]->strBoneName;

		for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
		{
			tMTKeyFrame tKeyframe = {};
			tKeyframe.fTime = (float)(vecBone[i]->vecKeyFrame[j].dTime);
			tKeyframe.iFrame = j;
			tKeyframe.vTranslate.x = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0]);
			tKeyframe.vTranslate.y = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1]);
			tKeyframe.vTranslate.z = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2]);

			tKeyframe.vScale.x = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0]);
			tKeyframe.vScale.y = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1]);
			tKeyframe.vScale.z = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2]);

			tKeyframe.qRot.x = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0]);
			tKeyframe.qRot.y = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1]);
			tKeyframe.qRot.z = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2]);
			tKeyframe.qRot.w = (float)(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3]);

			bone.vecKeyFrame.push_back(tKeyframe);
		}

		pMesh->m_vecBones.push_back(bone);
	}

	vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

	for (UINT i = 0; i < vecAnimClip.size(); ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = vecAnimClip[i]->strName;
		tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
		tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
		tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

		tClip.iStartFrame = (int)(vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode));
		tClip.iEndFrame = (int)(vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode));
		tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
		tClip.eMode = vecAnimClip[i]->eMode;

		pMesh->m_vecAnimClip.push_back(tClip);
	}

	return pMesh;
}


void CMesh::render(UINT _iSubSet)
{
	// Mesh 가 보유하고 있는 SubSet 개수(인덱스 버퍼 개수) 를 넘어서서 인덱스 접근하는 경우
	if (_iSubSet >= m_vecIdxInfo.size())
		assert(nullptr);

	// 정점 버퍼를 전달	
	UINT iOffset = 0;

	CONTEXT->IASetInputLayout(m_pLayout); // 정점 구조 알리기
	CONTEXT->IASetVertexBuffers(0, 1, &m_pVB, &m_iVtxSize, &iOffset);

	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubSet].pIB, m_vecIdxInfo[_iSubSet].eFormat, 0);
	CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubSet].iIdxCount, 0, 0);
}


void CMesh::render_instancing(UINT _iSubset)
{
	if (_iSubset >= m_vecIdxInfo.size())
		assert(nullptr);

	CONTEXT->IASetInputLayout(m_pLayout);

	ID3D11Buffer* arrBuffer[2] = { m_pVB	, CinstancingBuffer::GetInst()->GetBuffer() };
	UINT		  iStride[2] = { m_iVtxSize	, sizeof(tInstancingData) };
	UINT		  iOffset[2] = { 0, 0 };

	CONTEXT->IASetVertexBuffers(0, 2, arrBuffer, iStride, iOffset);

	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB, m_vecIdxInfo[_iSubset].eFormat, 0);
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_iSubset].iIdxCount
		, CinstancingBuffer::GetInst()->GetInstanceCount(), 0, 0, 0);
}

void CMesh::render_particle(UINT _iParticleCount)
{
	UINT iOffset = 0;
	CONTEXT->IASetInputLayout(m_pLayout);
	CONTEXT->IASetVertexBuffers(0, 1, &m_pVB, &m_iVtxSize, &iOffset);

	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[0].pIB, m_vecIdxInfo[0].eFormat, 0);
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _iParticleCount, 0, 0, 0);
}


void CMesh::SetLayout(CResPtr<CShader> _pShader, bool _bInstancing)
{
	static ID3DBlob* pBlob = nullptr;

	if (_bInstancing)
		pBlob = _pShader->GetVSInstBlob();
	else
		pBlob = _pShader->GetVSBlob();

	if (m_pVtxBlob == pBlob)
		return;

	m_pVtxBlob = _pShader->GetVSBlob();

	SAFE_RELEASE(m_pLayout);
	DEVICE->CreateInputLayout(&m_vecLayout[0]
		, m_vecLayout.size()
		, m_pVtxBlob->GetBufferPointer()
		, m_pVtxBlob->GetBufferSize()
		, &m_pLayout);
}

void CMesh::AddInputLayout(const char * _pSemanticName
	, UINT _iSemanticIdx, DXGI_FORMAT _eFormat
	, UINT _iInputSlot, UINT _iInstancing, D3D11_INPUT_CLASSIFICATION _eClass)
{
	D3D11_INPUT_ELEMENT_DESC tDesc = {};

	tDesc.SemanticName = _pSemanticName;
	tDesc.SemanticIndex = _iSemanticIdx; // 같은 종류의 시멘틱에 대해서 증가하는 인덱스
	tDesc.Format = _eFormat;
	tDesc.InputSlot = _iInputSlot; // 0 ~ 15 사이의 값
	tDesc.InputSlotClass = _eClass;
	tDesc.InstanceDataStepRate = _iInstancing; // 인스턴싱

	if (0 == _iInputSlot)
	{
		tDesc.AlignedByteOffset = m_iOffset;
		m_iOffset += GetSizeofFormat(_eFormat);
	}
	else
	{
		tDesc.AlignedByteOffset = m_iOffsetInst;
		m_iOffsetInst += GetSizeofFormat(_eFormat);
	}

	m_vecLayout.push_back(tDesc);
}

void CMesh::Save()
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += GetPath();

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	if (pFile == nullptr)
		return;

	// 키값
	SaveWString(GetName().c_str(), pFile);

	// 상대 경로
	SaveWString(GetPath().c_str(), pFile);

	// 정점 데이터 저장	
	fwrite(&m_iVtxSize, sizeof(int), 1, pFile);
	fwrite(&m_iVtxCount, sizeof(int), 1, pFile);

	int iByteSize = m_iVtxSize * m_iVtxCount;
	fwrite(&iByteSize, sizeof(int), 1, pFile);
	fwrite(m_pVtxMem, iByteSize, 1, pFile);
	fwrite(&m_tVtxDesc, sizeof(D3D11_BUFFER_DESC), 1, pFile);

	// 인덱스 정보
	UINT iMtrlCount = m_vecIdxInfo.size();
	fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
		fwrite(m_vecIdxInfo[i].pIdxSysMem, m_vecIdxInfo[i].tIdxDesc.ByteWidth, 1, pFile);
	}

	// Animation3D 정보 
	UINT iCount = m_vecAnimClip.size();
	fwrite(&iCount, sizeof(int), 1, pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(m_vecAnimClip[i].strAnimName.c_str(), pFile);
		fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].eMode, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
		fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
	}

	iCount = m_vecBones.size();
	fwrite(&iCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(m_vecBones[i].strBoneName.c_str(), pFile);
		fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		int iFrameCount = m_vecBones[i].vecKeyFrame.size();
		fwrite(&iFrameCount, sizeof(int), 1, pFile);

		for (UINT j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
		{
			fwrite(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}
	}

	fclose(pFile);
}

void CMesh::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// 키값
	SetName(LoadWString(pFile));

	// 상대경로
	SetPath(LoadWString(pFile));

	// 로드
	// 정점데이터
	fread(&m_iVtxSize, sizeof(int), 1, pFile);
	fread(&m_iVtxCount, sizeof(int), 1, pFile);

	UINT iByteSize = 0;
	fread(&iByteSize, sizeof(int), 1, pFile);

	m_pVtxMem = malloc(iByteSize);
	fread(m_pVtxMem, 1, iByteSize, pFile);

	D3D11_BUFFER_DESC tDesc = {};
	fread(&m_tVtxDesc, sizeof(D3D11_BUFFER_DESC), 1, pFile);

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = m_pVtxMem;

	if (FAILED(DEVICE->CreateBuffer(&m_tVtxDesc, &tSubData, &m_pVB)))
	{
		assert(nullptr);
	}

	// 인덱스 정보
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info, sizeof(tIndexInfo), 1, pFile);

		void* pSysMem = malloc(info.tIdxDesc.ByteWidth);
		info.pIdxSysMem = pSysMem;
		fread(info.pIdxSysMem, info.tIdxDesc.ByteWidth, 1, pFile);

		tSubData.pSysMem = info.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&info.tIdxDesc, &tSubData, &info.pIB)))
		{
			assert(nullptr);
		}
		m_vecIdxInfo.push_back(info);
	}

	// Layout 정보 넣기
	AddInputLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	AddInputLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	AddInputLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddInputLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);

	AddInputLayout("WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	AddInputLayout("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	/*인스턴싱 데이터는 슬롯이 다르다*/
	AddInputLayout("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	AddInputLayout("WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	AddInputLayout("ROWINDEX", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	// Animation3D 정보 읽기
	
	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	for (int i = 0; i < iCount; ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = LoadWString(pFile);
		fread(&tClip.dStartTime, sizeof(double), 1, pFile);
		fread(&tClip.dEndTime, sizeof(double), 1, pFile);
		fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
		fread(&tClip.eMode, sizeof(int), 1, pFile);
		fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
		fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
		fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
		fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

		m_vecAnimClip.push_back(tClip);
	}


	iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecBones.resize(iCount);

	for (int i = 0; i < iCount; ++i)
	{
		m_vecBones[i].strBoneName = LoadWString(pFile);
		fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		int iFrameCount = 0;
		fread(&iFrameCount, sizeof(int), 1, pFile);
		m_vecBones[i].vecKeyFrame.resize(iFrameCount);
		for (int j = 0; j < iFrameCount; ++j)
		{
			fread(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}
	}

	// Animation 이 있는 Mesh 경우 BoneTexture 만들어두기
	if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
	{
		wstring strBoneTex = GetName();
		strBoneTex += L"BoneTex";

		m_pBoneTex =
			CResMgr::GetInst()->CreateTexture(strBoneTex
				, GetBones()->size() * 4, 1
				, D3D11_BIND_SHADER_RESOURCE
				, D3D11_USAGE_DYNAMIC
				, DXGI_FORMAT_R32G32B32A32_FLOAT);

	}


	fclose(pFile);
}


bool CMesh::LoadFromScene(FILE * _pFile)
{
	CResource::LoadFromScene(_pFile);

	// 키값, 경로
	CResPtr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(GetName());

	if (nullptr != pMesh)
		return false;

	wstring strPath = CPathMgr::GetResPath();
	strPath += GetPath();
	Load(strPath);

	return true;
}
