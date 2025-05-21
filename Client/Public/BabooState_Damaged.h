#pragma once

#include "BabooState.h"

BEGIN(Client)

class CBabooState_Damaged : public CBabooState
{
public:
	CBabooState_Damaged(weak_ptr<class CMonster_Baboo> pBaboo, State preState, const DamageInfo& damageInfo, _bool isGroggy = false);

public:
	CBabooState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	_bool m_isGroggy = false;
	DamageInfo m_damageInfo;

private:
	string m_strCurAnim;
};

END