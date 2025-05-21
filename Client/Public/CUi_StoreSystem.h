#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StoreSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_StoreSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	virtual void Set_Active(_bool Active) override;


private:
	void Initialize_Menu();
	void Initialize_BackGround();
	void Initialize_Opening();
	void Initialize_StoreDefaultBack();
	void Check_Menu();
	void Select_Menu();
	void Opening(_float fTimeDelta);
	void Alpha_Adjust(_float fTimeDelta);


private:
	vector<shared_ptr<class CUi_StoreEntry>> m_MenuVec;
	vector<shared_ptr<class CUi_Default>> m_BackGroundVec;
	vector<shared_ptr<class CUi_Default>> m_pOpeningVec;
	_uint m_iFocusIndex = { 0 };
	_float m_KeyOverlapPreventTime = { 0 };
	_float m_fOpeningTime = { 0 };
	_float m_fAlphaTime = { 0 };
	_uint m_iTestRotation = { 0 };
	

public:
	static shared_ptr<CUi_StoreSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
