#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CTrigger : public CGameObject
{
public:
	static wstring ObjID;

protected:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& rhs);
public:
	virtual ~CTrigger();

public:
	void Set_LocalMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_originMat, Matrix); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
#ifdef _DEBUG
	virtual HRESULT Render();
#endif
	
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) {}
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) {}
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) {}

	void Save_Data(const string& strBinFilePath) {}
	void Load_Data(const string& strBinFilePath) {}

public:
	void SetUp_WorldMatrix(_fmatrix ParentMatrix);

protected:
	_uint m_iColliderType;
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_float4x4 m_originMat = {};

public:
	static shared_ptr<CTrigger> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END