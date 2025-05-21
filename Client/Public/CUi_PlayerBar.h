#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_PlayerBar : public CUi_2D
{
public:
	static wstring ObjID;

protected:
	CUi_PlayerBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PlayerBar(const CUi_2D& rhs);

public:
	virtual ~CUi_PlayerBar();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

protected:
	virtual HRESULT Ready_Components(void* pArg) override;

private:
	void Create_PlayerGuiter();
	void Create_PlayerHP();
	void Create_PlayerReverb();
	void Create_PlayerPartner();
	void Move_Shadow(_float fTimeDelta);

private:
	_float m_fShadowMoveTime = 0.5f;
	_float m_fShadowCurTime = 0.0f;
	_bool  m_bReturning = false;

private:
	shared_ptr<class CUi_PlayerGuiter> m_pGuiter;
	shared_ptr<class CUi_PlayerGuiter> m_pGuiterBackground;
	shared_ptr<class CUi_PlayerGuiter> m_pGuiterShadow;
	shared_ptr<class CUi_PlayerGuiter> m_pLock;

	shared_ptr<class CUi_PlayerHP> m_pHP;
	//shared_ptr<class CUi_PlayerHP> m_pDamage;
	shared_ptr<class CUi_PlayerHP> m_pHPBar;
	shared_ptr<class CUi_PlayerHP> m_pHPShadow;

	shared_ptr<class CUi_PlayerPartner> m_pPartnerPeppermint;
	
	shared_ptr<class CUi_PlayerReverb> m_pReverb1;
	shared_ptr<class CUi_PlayerReverb> m_pReverb2;
	shared_ptr<class CUi_PlayerReverb> m_pReverbBar1;
	shared_ptr<class CUi_PlayerReverb> m_pReverbBar2;
	/*shared_ptr<class CUi_PlayerReverb> m_pReverbOutline;*/
	shared_ptr<class CUi_PlayerReverb> m_pReverbShadow;

public:
	static shared_ptr<CUi_PlayerBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
