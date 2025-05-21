#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_ScoreSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_ScoreSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_ScoreSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_ScoreSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


protected:
	_float2 Change_APICoordinate(_float2 DirectPos);
	virtual void RenderFont(FontDescInfo Desc, _uint EnterGap = 20, _uint EnterCount = 0) override;


private:
	void Check_PlayerScore();
	void StringMove(_float fTimeDelta);
	void Initialize_ScoreBack();


private:
	shared_ptr<class CUi_Default> m_pScoreBack;
	_uint m_iPrevScore = { 0 };
	_bool m_bChange = { false };
	_float m_fMoveTime = { 0 };
	_float2 m_fOriginPos = { 560, 90 };
	_float m_fSpeed = { 300 };
	

public:
	static shared_ptr<CUi_ScoreSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
