#pragma once

#include "Client_Defines.h"
#include "CUi_PlayerBar.h"

BEGIN(Client)

class CUi_PlayerPartner : public CUi_PlayerBar
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_PartnerInfo : public CUi_2DInfo
	{
		_int iTextureNum;
	};

protected:
	CUi_PlayerPartner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PlayerPartner(const CUi_PlayerBar& rhs);

public:
	virtual ~CUi_PlayerPartner();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

public:
	static shared_ptr<CUi_PlayerPartner> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
