#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_RankSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_RankSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_RankSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_RankSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	virtual void Respawn() override;


private:
	void Check_PlayerCombo();
	void Cal_PlayerCombo(_uint iDigit);
	void Check_PlayerScore();
	void Cal_PlayerScore(_uint iDigit);
	void Check_PlayerRank();
	void Set_Beatmeter();
	void Set_BeatChange(_float fTimeDelta);
	void BeatChangeProgress(_float fTimeDelta);


private:
	void Initialize_Rank();
	void Initialize_ComboNum();
	void Initialize_ComboWord();
	void Initialize_ComboWordHit();
	void Initialize_Beatmeter();
	

private:
	shared_ptr<class CUi_Default> m_pRank;
	shared_ptr<class CUi_ComboWordHits> m_pWordHits;
	vector<shared_ptr<class CUi_ComboNum>> m_pComboNumVec;
	_float3 m_ComboNumVecOriginPos = { 0, 0, 0.7f };
	_uint m_iBeatChangeCheck = { 0 };
	_bool m_bBeatChange = { false };
	_float m_fBeatTimeCount = { 0 };
	_float2 m_fRankPos = { 500, 250 };
	_float2 m_fRankSize = { 150, 150 };


private:
	shared_ptr<class CUi_Default> m_pBeatmeter;
	shared_ptr<class CUi_Default> m_pBeatmeterBack;
	vector<shared_ptr<class CUi_Default>> m_pBeatmeterSpikeVec;
	shared_ptr<class CUi_Default> m_pBeatmeterSpikeBack;


public:
	static shared_ptr<CUi_RankSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
