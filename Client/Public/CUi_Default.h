#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_Default : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_Default(const CUi_2D& rhs);


public:
	virtual ~CUi_Default();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	void Initialize_BeatmeterResource();
	void Initialize_BeatMarkerResource();
	void Initialize_RankResource();
	void Initialize_ScoreBackResource();
	void Initialize_FinalResultBackResource();
	void Initialize_StageResultResource();
	void Initialize_HpResource();
	void Initialize_GearResource();
	void Initialize_StoreResource();


public:
	static shared_ptr<CUi_Default> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
