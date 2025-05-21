#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Client)

class CCombat_Manager
{
	DECLARE_SINGLETON(CCombat_Manager)

private:
	CCombat_Manager();

public:
	~CCombat_Manager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);
	void Render();

	void Register_Enemy(shared_ptr<CGameObject> pEnemy) { m_vecEnemy.push_back(pEnemy); }
	vector<shared_ptr<CGameObject>> Get_Enemy_Vector() { return m_vecEnemy; }
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<shared_ptr<CGameObject>> m_vecEnemy = {};
public:
	void Free();
};

END