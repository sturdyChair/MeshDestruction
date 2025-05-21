#include "..\Public\Bounding.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CBounding::~CBounding()
{
	Free();
}

HRESULT CBounding::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	return S_OK;
}

void CBounding::Update(_fmatrix WorldMatrix)
{

}


HRESULT CBounding::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	return S_OK;
}

void CBounding::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}