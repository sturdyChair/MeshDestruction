#pragma once

#include "GunnerState.h"

BEGIN(Client)

class CGunnerState_Move : public CGunnerState
{
public:
	CGunnerState_Move(weak_ptr<class CMonster_Gunner> pGunner, State preState);

public:
	CGunnerState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;
};

END