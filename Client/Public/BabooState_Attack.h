#pragma once

#include "BabooState.h"

BEGIN(Client)

class CBabooState_Attack : public CBabooState
{
public:
	CBabooState_Attack(weak_ptr<class CMonster_Baboo> pBaboo, State preState);

public:
	CBabooState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	void SelectAttack();

private:
	string m_strCurAnim;

public:
	void JumpOn();
	void JumpOff();

private:
	_bool m_isJumpMode = false;

private:
	_float m_fSpeedFixed = 0.f;
};

END