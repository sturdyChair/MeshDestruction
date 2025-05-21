#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_FinalResult : public CUi_2D
{
public:
	static wstring ObjID;


public:
	enum class FontType { Corus, Score, JustTiming, ClearTime, Bonus, End };
	using Desc = struct CUi_FinalResultInfo : public CUi_2DInfo
	{
		FontType eFontType = { FontType::End };
	};

protected:
	CUi_FinalResult(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_FinalResult(const CUi_2D& rhs);


public:
	virtual ~CUi_FinalResult();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	void Appear(_float fTimeDelta);
	void Disappear(_float fTimeDelta);


private:
	void Initialize_Font();
	void Initialize_Texture();
	void Initialize_BackGround();
	void Adjust_FontPos();
	void Adjust_BackPos();
	void Create_Rank();
	void Create_RankString();
	void Create_FinalResult();


private:
	shared_ptr<class CUi_Default> m_pBack = {};
	FontDescInfo m_SecondFontDesc = {};
	_float m_fScore = { 0 };
	_float m_fAppearTime = { 0 };
	_float m_fDisappearTime = { 0 };
	_float m_fAcc = { 0 };
	_float m_fScaleAcc = { 0 };
	_float m_fSpeedAcc = { 0 };
	_bool m_bBackAdjust = { false };
	FontType m_eFontType = { FontType::End };
	shared_ptr<class CUi_Default> m_pRank;
	shared_ptr<class CUi_Default> m_pRankString;
	shared_ptr<class CUi_Default> m_pFinalResult;
	_float2 m_RankSize = { 60, 60 };


public:
	static shared_ptr<CUi_FinalResult> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
