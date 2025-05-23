#pragma once

#include "SaberState.h"

BEGIN(Client)

class CSaberState_Idle : public CSaberState
{
public:
	CSaberState_Idle(weak_ptr<class CMonster_Saber> pSaber, State preState);

public:
	CSaberState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;
};

END