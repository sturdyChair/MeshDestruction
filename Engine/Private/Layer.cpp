#include "..\Public\Layer.h"
#include "GameObject.h"
#include "GameInstance.h"

CLayer::CLayer()
{
	m_pGameInstance = CGameInstance::Get_Instance();
}

CLayer::~CLayer()
{
	Free();
}

HRESULT CLayer::Add_GameObject(shared_ptr<CGameObject>  pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

void CLayer::PriorityTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && !pGameObject->Is_Dead())
			pGameObject->PriorityTick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && !pGameObject->Is_Dead())
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::LateTick(_float fTimeDelta)
{
	auto iter = m_GameObjects.begin();
	for (iter; iter != m_GameObjects.end();)
	{
		if ((*iter)->Is_Dead())
		{
			(*iter)->Dying();
			m_pGameInstance.lock()->Push_Dead_Obj(*iter);
			iter = m_GameObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (auto& pGameObject : m_GameObjects)
	{
		pGameObject->LateTick(fTimeDelta);
	}
}

shared_ptr<CComponent> CLayer::Find_Component(const wstring& strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter)->Find_Component(strComponentTag);
}

shared_ptr<CLayer> CLayer::Create()
{
	MAKE_SHARED_ENABLER(CLayer)


	return make_shared<MakeSharedEnabler>();
}

void CLayer::Free()
{
	//for (auto& pGameObject : m_GameObjects)
	//	Safe_Release(pGameObject);

	m_GameObjects.clear();

}
