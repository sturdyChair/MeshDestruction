#include "..\Public\GameObject.h"
#include "GameInstance.h"


const _wstring	CGameObject::m_strTransformTag = TEXT("Com_Transform");
_uint CGameObject::s_iID = 0;
unordered_map<_uint, CGameObject*> CGameObject::s_HashedMap;

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	m_iObjectID = s_iID;
	s_HashedMap.emplace(m_iObjectID, this);
	++s_iID;
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
	, m_pGameInstance{ rhs.m_pGameInstance }
	, m_iLevelIndex{ rhs.m_iLevelIndex }
{
	m_iObjectID = s_iID;
	s_HashedMap.emplace(m_iObjectID, this);
	++s_iID;
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

CGameObject::~CGameObject()
{
	Free();
}

HRESULT CGameObject::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	/* 원형을 생서하는 것처럼 보이지만. 원형이 아니고.  */
	/* 복제된 객체가 활용하기위한 실제 사용을 위한 트랜스폼을 만든다. */
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	/* 타 객체가 나의 트랜스폼을 검색할 수 있도록 맵에 추가하낟. */
	if (nullptr != Find_Component(m_strTransformTag))
		return E_FAIL;

	m_Components.emplace(m_strTransformTag, m_pTransformCom);

	if (nullptr != pArg)
	{
		if (FAILED(m_pTransformCom->Initialize(pArg)))
			return E_FAIL;
	}


	return S_OK;
}

void CGameObject::PriorityTick(_float fTimeDelta)
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}


shared_ptr<CComponent> CGameObject::Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, void* pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return nullptr;

	shared_ptr<CComponent> pComponent = m_pGameInstance.lock()->Clone_Component(iPrototoypeLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return nullptr;
	pComponent->Set_Owner(shared_from_this());
	m_Components.emplace(strComponentTag, pComponent);

	return  pComponent;

}

shared_ptr<CComponent> CGameObject::Find_Component(const wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Dead()
{
	if (m_bIsDead)
		return;
	DELAYED event{};
	event.eType = DELAYED_TYPE::DELETE_OBJECT;
	event.wParam = (DWORD_PTR)this;
	m_pGameInstance.lock()->Add_Delayed(event);
}

shared_ptr<CGameObject> CGameObject::Find_Object_With_ID(_uint iID)
{
	auto pObj = s_HashedMap[iID];
	if(pObj)
		return pObj->shared_from_this();

	return nullptr;
}

template<typename T>
inline shared_ptr<CComponent> CGameObject::Find_Component_By_Type()
{
	for (auto& iter : m_Components)
	{
		auto pComponent = iter.second;
		shared_ptr<T> rt = dynamic_pointer_cast<T>(pComponent);
		if (rt)
			return pComponent;
	}
	return shared_ptr<CComponent>();
}

shared_ptr<CGameObject> CGameObject::Clone(void* pArg, _uint iLevelIndex)
{
	m_iLevelIndex = iLevelIndex;
	return Clone(pArg);
}

void CGameObject::Free()
{

	auto iter = m_Components.begin();

	//if (s_HashedMap.find(m_iObjectID) != s_HashedMap.end())
	//	s_HashedMap.erase(m_iObjectID);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
