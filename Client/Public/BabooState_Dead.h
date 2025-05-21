#pragma once

#include "BabooState.h"

BEGIN(Client)

class CBabooState_Dead : public CBabooState
{
public:
	CBabooState_Dead(weak_ptr<class CMonster_Baboo> pBaboo, State preState, const DamageInfo& damageInfo);

public:
	CBabooState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	DamageInfo m_damageInfo;
};

END