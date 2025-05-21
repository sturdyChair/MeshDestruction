#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StageResultSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_StageResultSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StageResultSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_StageResultSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Check_Corus(_float fTimeDelta);
	void Create_Corus();

	void Check_Fade(_float fTimeDelta);
	void Fade_Out(_float fTimeDelta);
	void Create_Fade();


	void Check_Bonus(_float fTimeDelta);
	void Create_Bonus();

	void Check_FinalScore(_float fTimeDelta);
	void Create_FinalScore();

	void Check_Rank(_float fTimeDelta);
	void Create_Rank();
	void Adjust_Rank(_float fTimeDelta);

	void Check_RankString(_float fTimeDelta);
	void Create_RankString();
	void Adjust_RankString(_float fTimeDelta);

	void Initialize_Screen();
	void Initialize_Title();
	void Initialize_CorusVec();


public:
	shared_ptr<class CUi_Default> m_pFade;
	_float m_fFadeAlpha = { 0 };
	_float m_fCorusCreateGap = { 0 };
	_float m_fSystemTime = {0};
	_bool m_bFadeOut = { false };
	vector<_uint> m_CorusScoreVec;
	_uint m_CorusCreateCount = { 0 };

	_float m_fFontCreateTime = { 0 };
	_bool m_bCanCreateBonus = { false };
	_bool m_bCreateBonus = { false };


	_float m_fFinalScoreCreateTime = { 0 };
	_bool m_bCreateFinalScore = { false };

	shared_ptr<class CUi_Default> m_pRank;
	_float m_fRankCreateTime = { 0 };
	_bool m_bCreateRank = { false };
	_float2 m_fOriginRankScale = { 270, 270 };
	_bool m_bCreateRankString = { false };
	_float m_fRankStringCreateTime = { 0 };
	shared_ptr<class CUi_Default> m_pRankString;


public:
	static shared_ptr<CUi_StageResultSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
