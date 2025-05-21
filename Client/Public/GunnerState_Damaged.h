#pragma once

#include "GunnerState.h"

BEGIN(Client)

class CGunnerState_Damaged : public CGunnerState
{
public:
	CGunnerState_Damaged(weak_ptr<class CMonster_Gunner> pGunner, State preState, const DamageInfo& damageInfo);

public:
	CGunnerState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	DamageInfo m_damageInfo;
};

END