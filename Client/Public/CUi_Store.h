#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "CUi_StoreDesc.h"

BEGIN(Client)

class CUi_Store abstract: public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_Store(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_Store(const CUi_2D& rhs);


public:
	virtual ~CUi_Store();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	virtual void Set_Active(_bool Active) override;
	virtual void Dying() override;


protected:
	void Progress_Store(_float fTimeDelta);
	void Adjust_Alpha(_float fTimeDelta);
	void Select_Bar();
	void Return_Menu();
	void Initialize_StoreDesc();


public:
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;

protected:
	vector<shared_ptr<class CUi_StoreBar>> m_StoreBarVec;
	shared_ptr<class CUi_StoreDesc> m_StoreDesc;
	_int m_iFocusIndex = { 0 };

	vector<shared_ptr<class CUi_Default>> m_BackVec;
	_float m_fAdjustAlpha = { 0 };
	_float m_fBuyTime = { 0 };


public:
	static shared_ptr<CUi_Store> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
