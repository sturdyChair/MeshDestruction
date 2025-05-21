#pragma once

#include "Client_Defines.h"
#include "CUi_Store.h"

BEGIN(Client)

class CUi_StoreAttack : public CUi_Store
{
public:
	static wstring ObjID;


protected:
	CUi_StoreAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreAttack(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreAttack();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

private:
	void Initialize_Attack();
	void Initialize_AttackMap();
	void Initialize_AttackBack();


private:
	map<wstring, CUi_StoreDesc::AttackDesc> m_AttackMap;
	_float m_fOriginAttackY = { 70 };


public:
	static shared_ptr<CUi_StoreAttack> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
