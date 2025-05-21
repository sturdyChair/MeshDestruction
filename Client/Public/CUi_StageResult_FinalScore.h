#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StageResult_FinalScore : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_StageResult_FinalScore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StageResult_FinalScore(const CUi_2D& rhs);


public:
	virtual ~CUi_StageResult_FinalScore();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Appear(_float fTimeDelta);
	void Initialize_Font();


private:
	_float m_fAppearTime = { 0 };
	FontDescInfo m_ScoreFont;


public:
	static shared_ptr<CUi_StageResult_FinalScore> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
