#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent{ rhs }
	, m_pVB{ rhs.m_pVB }
	, m_pIB{ rhs.m_pIB }
	, m_iVertexStride{ rhs.m_iVertexStride }
	, m_iIndexStride{ rhs.m_iIndexStride }
	, m_iNumVertices{ rhs.m_iNumVertices }
	, m_iNumIndices{ rhs.m_iNumIndices }
	, m_iNumVertexBuffers{ rhs.m_iNumVertexBuffers }
	, m_eIndexFormat{ rhs.m_eIndexFormat }
	, m_ePrimitiveTopology{ rhs.m_ePrimitiveTopology }
	, m_vecVertexPositions{ rhs.m_vecVertexPositions }
	, m_vecIndicies{ rhs.m_vecIndicies }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

CVIBuffer::~CVIBuffer()
{
	Free();
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint					iStrides[] = {
		m_iVertexStride,
	};

	_uint					iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Draw()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffer()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	};

	_uint					iStrides[] = {
		m_iVertexStride,
	};

	_uint					iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	return S_OK;
}

HRESULT CVIBuffer::Bind_VB()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint					iStrides[] = {
		m_iVertexStride,
	};

	_uint					iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	return S_OK;
}

HRESULT CVIBuffer::Bind_IB(_uint iIdx)
{
	if (iIdx >= m_vecIB.size())
		return E_FAIL;
	m_pContext->IASetIndexBuffer(m_vecIB[iIdx], m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppOut)
{
	/* 1. D3D11_BUFFER_DESC : ���۸� �����ϱ����� ���� ����(�����Ʈ�Ҵ��Ѱ���? �Ӽ��� ��, ����. */
	/* 2. D3D11_SUBRESOURCE_DATA : ���� ������ ������ � ������ ä��������? */
	/* 3. Out */
	m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppOut);

	return S_OK;
}

void CVIBuffer::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
