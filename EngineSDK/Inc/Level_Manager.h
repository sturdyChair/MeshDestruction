#pragma once

#include "Engine_Defines.h"

/* 1. ���� �Ҵ�� ������ �����ϳ�.*/
/* 1_1. ���� ��ü ��, ���� ������ �ı��ϴ� ��Ȱ. ���� ���������� �Ҵ�ƴ� �޸𸮵� �ؼ��ϳ�.  */
/* 2. Ȱ��ȭ�� ������ Tick Render�� ������ ȣ���Ѵ�. */


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