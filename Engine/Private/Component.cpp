#include "..\Public\Component.h"
#include "GameInstance.h"

CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pGameInstance(CGameInstance::Get_Instance())
	, m_isCloned{ false }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent& rhs)
	: m_pDevice{ rhs.m_pDevice }, m_pContext{ rhs.m_pContext }, m_pGameInstance(rhs.m_pGameInstance)
	, m_isCloned{ true }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::~CComponent()
{
	Free();
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

shared_ptr<class CGameObject> CComponent::Get_Owner()
{
	if (m_pOwner.expired())
		return nullptr;
	return m_pOwner.lock();
}

void CComponent::Set_Owner(shared_ptr<class CGameObject> pOwner)
{
	m_pOwner = pOwner;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice); Safe_Release(m_pContext);
}
