#pragma once
#include "Engine_Defines.h"
/* 사본객체들을 모아서 저장한다. */
/* 4. 보유하고있는 사본객체들의 Tick들을 호출한다. */
/* 5. 보유하고있는 사본객체들의 Render (x : 그리는 순서를 관리해야 할 필요가 있어서. ) */

BEGIN(Engine)

class CLayer 
{
private:
	CLayer();
public:
	virtual ~CLayer();

public:
	HRESULT Add_GameObject(shared_ptr<class CGameObject> pGameObject);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	list<shared_ptr<class CGameObject>>& Get_ObjectList() { return m_GameObjects; }

public:
	shared_ptr<class CComponent> Find_Component(const wstring& strComponentTag, _uint iIndex);

private:
	list<shared_ptr<class CGameObject>>			m_GameObjects;
	weak_ptr<class CGameInstance> m_pGameInstance;
public:
	static shared_ptr<CLayer> Create();
	void Free();
};

END