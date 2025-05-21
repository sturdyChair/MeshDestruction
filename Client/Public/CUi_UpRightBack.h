#pragma once

#include "Client_Defines.h"
#include "CUi_Default.h"

BEGIN(Client)

class CUi_UpRightBack : public CUi_Default
{
public:
	static wstring ObjID;


protected:
	CUi_UpRightBack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_UpRightBack(const CUi_2D& rhs);


public:
	virtual ~CUi_UpRightBack();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	virtual void Set_Color(_float4 Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	virtual void Add_Color(_float4 Color) override;
	virtual void Add_Color(ColorType Type, _float Color) override;


public:
	void Set_Font();
	virtual void Set_Active(_bool Active) override;


private:
	void Initialize_Gear();
	void Adjust_Alpha(_float fTimeDelta);


private:
	shared_ptr<class CUi_Default> m_pGear;
	_float m_fAlphaTime = { 0 };


public:
	static shared_ptr<CUi_UpRightBack> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
