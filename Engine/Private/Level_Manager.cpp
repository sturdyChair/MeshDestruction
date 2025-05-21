#include "..\Public\Level_Manager.h"
#include "Level.h"

#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	
}

CLevel_Manager::~CLevel_Manager()
{
	Free();
}

HRESULT CLevel_Manager::Change_Level(shared_ptr<CLevel> pNewLevel)
{
	/* 기존 레벨용으로 할당됐던 메모리도 해소하낟.  */
	if (nullptr != m_pCurrentLevel)
	{
		m_pGameInstance.lock()->Clear(m_pCurrentLevel->Get_LevelID());

	}

	m_pCurrentLevel = pNewLevel;

	return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	m_pCurrentLevel->Render();

	return S_OK;
}

_uint CLevel_Manager::Get_Current_LevelID() const
{
	return m_pCurrentLevel->Get_LevelID();
}

shared_ptr<CLevel_Manager> CLevel_Manager::Create()
{
	MAKE_SHARED_ENABLER(CLevel_Manager)
	return make_shared<MakeSharedEnabler>();
}

void CLevel_Manager::Free()
{
}


