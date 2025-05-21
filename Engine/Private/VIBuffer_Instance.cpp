#include "VIBuffer_Instance.h"
#include "GameInstance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CVIBuffer(pDevice, pContext)

{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs) :
	CVIBuffer(rhs)
	//, m_pVBInstance(rhs.m_pVBInstance)
	, m_tDesc(rhs.m_tDesc)
	, m_iInstanceVertexStride(rhs.m_iInstanceVertexStride)
	, m_iNumIndexPerInstance(rhs.m_iNumIndexPerInstance)
	, m_pSpeeds(rhs.m_pSpeeds)
	, m_vecInitPos(rhs.m_vecInitPos)
	, m_vecTorque(rhs.m_vecTorque)
{
}

CVIBuffer_Instance::~CVIBuffer_Instance()
{
	Free();
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const INSTANCE_DESC* desc)
{
	m_tDesc = *desc;

	m_pSpeeds = new _float[m_tDesc.iNumInstance];
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		m_pSpeeds[i] = GAMEINSTANCE->Random_Float(m_tDesc.vSpeed.x, m_tDesc.vSpeed.y);
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint					iStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_tDesc.iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Buffer()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	m_pVBInstance
	};

	_uint					iStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	return S_OK;
}

HRESULT CVIBuffer_Instance::Draw()
{
	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_tDesc.iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instance::Speed_Renewal(_float _fXSpeed, _float _fYSpeed)
{
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		m_pSpeeds[i] = GAMEINSTANCE->Random_Float(_fXSpeed, _fYSpeed);
	}

	Reset();
	m_tDesc.vSpeed = _float2{ _fXSpeed, _fYSpeed };
}

void CVIBuffer_Instance::Range_Renewal(_float3 vRange)
{
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		m_vecInitPos[i].x = GAMEINSTANCE->Random_Float(-vRange.x, vRange.x);
		m_vecInitPos[i].y = GAMEINSTANCE->Random_Float(-vRange.y, vRange.y);
		m_vecInitPos[i].z = GAMEINSTANCE->Random_Float(-vRange.z, vRange.z);
	}

	Reset();
	m_tDesc.vRange = vRange;
}

void CVIBuffer_Instance::Scale_Renewal(_float2 _vScale)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	_matrix Transform = XMMatrixIdentity();
	//_matrix Rotation = XMMatrixIdentity();
	for (_uint i = 0; i < m_tDesc.iNumInstance; ++i)
	{
		_float fScale = GAMEINSTANCE->Random_Float(_vScale.x, _vScale.y);

		Transform = XMMatrixScaling(fScale, fScale, 1.f);
		//Transform = Rotation * Transform;

		XMStoreFloat4(&pVertices[i].vRight, Transform.r[0]);
		XMStoreFloat4(&pVertices[i].vUp, Transform.r[1]);
		XMStoreFloat4(&pVertices[i].vLook, Transform.r[2]);
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	Reset();
	m_tDesc.vScale = _vScale;
}

void CVIBuffer_Instance::Free()
{
	Safe_Release(m_pVBInstance);
	if (!m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_vecInitPos);
		Safe_Delete_Array(m_vecTorque);
	}
}
