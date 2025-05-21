#pragma once

#include "SaberState.h"

BEGIN(Client)

class CSaberState_Move : public CSaberState
{
public:
	CSaberState_Move(weak_ptr<class CMonster_Saber> pSaber, State preState);

public:
	CSaberState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	void SelectMoveAnim();

private:
	_float m_fDistance_Close = 0.f;
	_float m_fDistance_Too_Close = 0.f;

private:
	_float m_fCurTimer = 0.f;
	_bool m_isActivingShort = false;

	_float4 m_vTargetPos;
};

END