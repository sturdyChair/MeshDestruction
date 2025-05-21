#include "..\Public\Component_Manager.h"

#include "Component.h"

CComponent_Manager::CComponent_Manager()
{
}

CComponent_Manager::~CComponent_Manager()
{
	Free();
}

HRESULT CComponent_Manager::Initialize(_uint iNumLevels)
{
	m_pPrototypes.resize(iNumLevels);

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<CComponent> pPrototype)
{
	if (iLevelIndex >= m_iNumLevels ||
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

shared_ptr<CComponent> CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	shared_ptr<CComponent> pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	shared_ptr<CComponent> pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;


	m_pPrototypes[iLevelIndex].clear();
}

shared_ptr<CComponent> CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

shared_ptr<CComponent_Manager> CComponent_Manager::Create(_uint iNumLevels)
{
	MAKE_SHARED_ENABLER(CComponent_Manager);
	shared_ptr<CComponent_Manager> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CComponent_Manager"));
		assert(false);
	}

	return pInstance;
}



void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		//for (auto& Pair : m_pPrototypes[i])
		//	Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}


}

