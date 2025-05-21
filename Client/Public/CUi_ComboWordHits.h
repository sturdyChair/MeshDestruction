#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_ComboWordHits : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_ComboWordHits(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_ComboWordHits(const CUi_2D& rhs);


public:
	virtual ~CUi_ComboWordHits();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	void Check_Change();
	void Scaling(_float fTimeDelta);


private:
	_float3 m_OriginScale = { 0, 0, 1};
	_uint m_iOriginCombo = {};
	_float m_fScaleTime = {};
	_bool m_bChange = { false };


public:
	static shared_ptr<CUi_ComboWordHits> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
