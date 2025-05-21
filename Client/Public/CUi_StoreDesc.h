#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StoreDesc : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using AttackDesc = struct AttackExplainInfo
	{
		wstring TitleFont = L"";
		wstring PriceFont = L"";
		wstring ExplainFont = L"";
		_uint iEnterCount = { 0 };
	};

	using ItemDesc = struct ItemExplainInfo
	{
		wstring TitleFont = L"";
		wstring PriceFont = L"";
		wstring ExplainFont = L"";
		_uint iEnterCount = { 0 };
	};

	using SkillDesc = struct SkillExplainInfo
	{
		wstring TitleFont = L"";
		wstring PriceFont = L"";
		wstring ExplainFont = L"";
		_uint iEnterCount = { 0 };
	};
protected:
	CUi_StoreDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreDesc(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreDesc();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Desc(AttackDesc AttackInfo);
	void Set_Desc(SkillDesc SkillInfo);
	void Set_Desc(ItemDesc ItemInfo);
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	void Adjust_Alpha(_float fTimeDelta);
	virtual void Set_Active(_bool Active) override;


private:
	void Initialize_Font();


private:
	FontDescInfo m_TitleFont;
	FontDescInfo m_ExplainFont;
	FontDescInfo m_PriceFont;
	FontDescInfo m_BuyFont;
	_uint m_iExplainFontEnterCount = { 0 };
	_float m_fAlphaTime = { 0 };


public:
	static shared_ptr<CUi_StoreDesc> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
