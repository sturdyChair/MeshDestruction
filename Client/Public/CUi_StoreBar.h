#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StoreBar : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_StoreBarInfo : public CUi_2DInfo
	{
		wstring StringFont;
		_uint Price = {};
	};

protected:
	CUi_StoreBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreBar(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	wstring Get_SkillName() const { return m_StringFont.String; }
	_uint Get_Price() const { return m_iPrice; }
	void Set_Focus(_bool Focus) { m_bFocus = Focus; }
	virtual void Set_Active(_bool Active) override;
	virtual void Dying() override;
	void Set_StringFont(wstring str);
	void Set_PriceFont(_uint Price);
	void Adjust_Alpha(_float fTimeDelta);
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;


private:
	void Initialize_DefaultVec();
	void Adjust_Pos(_float fTimeDelta);


private:
	FontDescInfo m_StringFont;
	_float2 m_fStringFont_Distance = {};
	_float2 m_fPriceFont_Distance = {};
	_uint m_iPrice = { 0 };
	_bool m_bFocus = { false };
	_float2 m_fOriginPos = { 0, 0 };
	vector<shared_ptr<class CUi_Default>> m_pDefaultVec;
	_float m_fAlphaTime = { 0 };


public:
	static shared_ptr<CUi_StoreBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
