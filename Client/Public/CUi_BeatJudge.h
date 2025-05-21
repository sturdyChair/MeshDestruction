#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "CUi_BeatMarkerSystem.h"

BEGIN(Client)

class CUi_BeatJudge : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_BeatJudgeInfo : public CUi_2DInfo
	{
		CUi_BeatMarkerSystem::JudgeType Type = { CUi_BeatMarkerSystem::JudgeType::Miss };
	};

protected:
	CUi_BeatJudge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_BeatJudge(const CUi_2D& rhs);


public:
	virtual ~CUi_BeatJudge();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	static shared_ptr<CUi_BeatJudge> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
