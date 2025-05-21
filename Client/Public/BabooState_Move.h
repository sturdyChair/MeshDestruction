#pragma once

#include "BabooState.h"

BEGIN(Client)

class CBabooState_Move : public CBabooState
{
public:
	CBabooState_Move(weak_ptr<class CMonster_Baboo> pBaboo, State preState);

public:
	CBabooState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	void SelectMoveAnim();

private:
	_uint m_iCurBeat;

	_int m_iTargetBeat = 3;
	_int m_iBeatCnt = 0;

private:
	_bool m_isLookStrong = false;

private:
	string m_strCurAnim;
};

END