#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StageResult_Bonus : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_StageResult_Bonus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StageResult_Bonus(const CUi_2D& rhs);


public:
	virtual ~CUi_StageResult_Bonus();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Initialize_Font();
	void Appear(_float fTimeDelta);
	void Adjust_FontColor(_float fTimeDelta);
	void Adjust_FontPos(_float fTimeDelta);
	void Cal_Time();


private:
	_float m_fAppearTime = { 0 };
	FontDescInfo m_SecondFont;
	FontDescInfo m_ThirdFont;
	FontDescInfo m_FourthFont;
	FontDescInfo m_StringFirstFont;
	FontDescInfo m_StringSecondFont;
	FontDescInfo m_StringThirdFont;
	FontDescInfo m_StringFourthFont;


public:
	static shared_ptr<CUi_StageResult_Bonus> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
