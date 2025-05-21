#pragma once

#include "GunnerState.h"

BEGIN(Client)

class CGunnerState_Attack : public CGunnerState
{
public:
	CGunnerState_Attack(weak_ptr<class CMonster_Gunner> pGunner, State preState);

public:
	CGunnerState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	string m_strCurAnim;

	const _float m_fSpeedRotation_C = 4.f;
};

END