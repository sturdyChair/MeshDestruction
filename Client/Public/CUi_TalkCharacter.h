#pragma once

#include "Client_Defines.h"
#include "CUi_TalkSystem.h"

BEGIN(Client)

class CUi_TalkCharacter : public CUi_TalkSystem
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_TalkCharInfo : public CUi_2DInfo
	{
		_int iTextureNum;
	};

protected:
	CUi_TalkCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TalkCharacter(const CUi_TalkSystem& rhs);

public:
	virtual ~CUi_TalkCharacter();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

public:
	static shared_ptr<CUi_TalkCharacter> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
