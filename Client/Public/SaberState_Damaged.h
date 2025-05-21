#pragma once

#include "SaberState.h"

BEGIN(Client)

class CSaberState_Damaged : public CSaberState
{
public:
	CSaberState_Damaged(weak_ptr<class CMonster_Saber> pSaber, State preState, const DamageInfo& damageInfo);

public:
	CSaberState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	DamageInfo m_damageInfo;
};

END