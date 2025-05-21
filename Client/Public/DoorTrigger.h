#pragma once
#include "Client_Defines.h"
#include "Trigger.h"

BEGIN(Client)

class CDoorTrigger : public CTrigger
{
public:
	static _wstring ObjID;

	struct DOORTRIGGER_DESC
	{
		shared_ptr<CGameObject> pDoorObject = { nullptr };
	};
	
private:
	CDoorTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoorTrigger(const CDoorTrigger& rhs);
public:
	~CDoorTrigger();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
#ifdef _DEBUG
	virtual HRESULT Render();
#endif

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

private:
	shared_ptr<CGameObject> m_pDoorObject = { nullptr };

public:
	static shared_ptr<CDoorTrigger> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END