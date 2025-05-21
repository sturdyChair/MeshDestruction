#include "..\Public\PartObject.h"


CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);

	m_pParentTransform = pDesc->pParentTransform;

	

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::PriorityTick(_float fTimeDelta)
{
}

void CPartObject::Tick(_float fTimeDelta)
{
}

void CPartObject::LateTick(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

void CPartObject::SetUp_WorldMatrix(_fmatrix ChildWorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, ChildWorldMatrix * m_pParentTransform->Get_WorldMatrix_XMMat());
}

void CPartObject::Free()
{

}
