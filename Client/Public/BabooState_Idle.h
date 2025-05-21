#pragma once

#include "BabooState.h"

BEGIN(Client)

class CBabooState_Idle : public CBabooState
{
public:
	CBabooState_Idle(weak_ptr<class CMonster_Baboo> pBaboo, State preState);

public:
	CBabooState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;
};

END