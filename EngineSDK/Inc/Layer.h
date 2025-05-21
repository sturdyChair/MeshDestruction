#pragma once
#include "Engine_Defines.h"
/* �纻��ü���� ��Ƽ� �����Ѵ�. */
/* 4. �����ϰ��ִ� �纻��ü���� Tick���� ȣ���Ѵ�. */
/* 5. �����ϰ��ִ� �纻��ü���� Render (x : �׸��� ������ �����ؾ� �� �ʿ䰡 �־. ) */

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