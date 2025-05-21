#include "..\Public\VIBuffer_RectNorm.h"

CVIBuffer_RectNorm::CVIBuffer_RectNorm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_RectNorm::CVIBuffer_RectNorm(const CVIBuffer_RectNorm& rhs)
	: CVIBuffer{ rhs }
{
}

CVIBuffer_RectNorm::~CVIBuffer_RectNorm()
{
	Free();
}

HRESULT CVIBuffer_RectNorm::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iIndexStride = 2;
	m_iNumVertices = 4;
	m_iNumIndices = 6;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	/* 생성하고자하는 버퍼의 속성을 설정하낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 생성하고자하는 버퍼의 초기값을 설정한다. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	pVertices[0].vPosition = XMFLOAT3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = XMFLOAT2(0.f, 0.f);
	pVertices[0].vNormal = XMFLOAT3(0.f, 0.f, 1.f);

	pVertices[1].vPosition = XMFLOAT3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = XMFLOAT2(1.f, 0.f);
	pVertices[1].vNormal = XMFLOAT3(0.f, 0.f, 1.f);

	pVertices[2].vPosition = XMFLOAT3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = XMFLOAT2(1.f, 1.f);
	pVertices[2].vNormal = XMFLOAT3(0.f, 0.f, 1.f);

	pVertices[3].vPosition = XMFLOAT3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = XMFLOAT2(0.f, 1.f);
	pVertices[3].vNormal = XMFLOAT3(0.f, 0.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_RectNorm::Initialize(void* pArg)
{
	return S_OK;
}

shared_ptr<CVIBuffer_RectNorm> CVIBuffer_RectNorm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CVIBuffer_RectNorm
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer_RectNorm(pDevice, pContext) { }
	};

	shared_ptr<CVIBuffer_RectNorm> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_RectNorm"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}


shared_ptr<CComponent> CVIBuffer_RectNorm::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CVIBuffer_RectNorm
	{
		MakeSharedEnabler(const CVIBuffer_RectNorm& rhs) : CVIBuffer_RectNorm(rhs) { }
	};

	shared_ptr<CVIBuffer_RectNorm> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_RectNorm"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_RectNorm::Free()
{
}
