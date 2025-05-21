#pragma once

#include "Client_Defines.h"
#include "CUi_PlayerBar.h"

BEGIN(Client)

class CUi_PlayerHP : public CUi_PlayerBar
{
public:
	static wstring ObjID;

protected:
	CUi_PlayerHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PlayerHP(const CUi_PlayerBar& rhs);


public:
	virtual ~CUi_PlayerHP();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

public:
	static shared_ptr<CUi_PlayerHP> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END