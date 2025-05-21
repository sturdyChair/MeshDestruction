#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_TalkSystem : public CUi_2D
{
public:
	static wstring ObjID;

protected:
	CUi_TalkSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TalkSystem(const CUi_2D& rhs);

public:
	virtual ~CUi_TalkSystem();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

protected:
	virtual HRESULT Ready_Components(void* pArg) override;

private:
	void Create_TalkRobotA();

private:
	shared_ptr<class CUi_TalkCharacter> m_pRobotA;

	shared_ptr<class CUi_TalkWindow> m_pBlackBoard;
	shared_ptr<class CUi_TalkWindow> m_pWindow0;
	shared_ptr<class CUi_TalkWindow> m_pWindow1;
	shared_ptr<class CUi_TalkWindow> m_pWindow2;
	shared_ptr<class CUi_TalkWindow> m_pWindow3;

public:
	static shared_ptr<CUi_TalkSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
