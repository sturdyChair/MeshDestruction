#pragma once

#include "Client_Defines.h"
#include "CUi_Store.h"

BEGIN(Client)

class CUi_StoreItem : public CUi_Store
{
public:
	static wstring ObjID;


protected:
	CUi_StoreItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreItem(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreItem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

private:
	void Initialize_Item();
	void Initialize_ItemMap();


private:
	map<wstring, CUi_StoreDesc::ItemDesc> m_ItemMap;


public:
	static shared_ptr<CUi_StoreItem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
