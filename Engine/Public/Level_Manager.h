#pragma once

#include "Engine_Defines.h"

/* 1. 현재 할당된 레벨을 보관하낟.*/
/* 1_1. 레벨 교체 시, 기존 레벨을 파괴하는 역활. 기존 레벨용으로 할당됐던 메모리도 해소하낟.  */
/* 2. 활성화된 레벨의 Tick Render를 무한히 호췰한다. */


BEGIN(Engine)

class CLevel_Manager
{
private:
	CLevel_Manager();
public:
	virtual ~CLevel_Manager();

public:
	HRESULT Change_Level(shared_ptr<class CLevel> pNewLevel);
	void Tick(_float fTimeDelta);
	HRESULT Render();
	_uint	Get_Current_LevelID() const;

private:
	shared_ptr<class CLevel> m_pCurrentLevel = { nullptr };
	weak_ptr<class CGameInstance> m_pGameInstance;

public:
	static shared_ptr<CLevel_Manager> Create();
	void Free();
};

END