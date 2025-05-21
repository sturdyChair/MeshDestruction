#pragma once

#include "Client_Defines.h"
#include "CUi_Store.h"

BEGIN(Client)

class CUi_StoreSkill : public CUi_Store
{
public:
	static wstring ObjID;


protected:
	CUi_StoreSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreSkill(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreSkill();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	
	
private:
	void Initialize_Skill();
	void Initialize_SkillMap();


private:
	map<wstring, CUi_StoreDesc::SkillDesc> m_SkillMap;
	_float m_fOriginSkillY = { 300 };

	
public:
	static shared_ptr<CUi_StoreSkill> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
