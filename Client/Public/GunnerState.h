#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CGunnerState abstract
{
public:
	enum class State {
		IDLE, MOVE, ATTACK, DAMAGED, DEAD, END_STATE
	};

public:
	virtual CGunnerState* StepState(_float fTimeDelta) PURE;
	virtual void ActiveState(_float fTimeDelta) PURE;

	void ChangeState(CGunnerState* pCurState, CGunnerState* pNextState);

	virtual void EnterState() PURE;
	virtual void ExitState() PURE;

protected:
	weak_ptr<class CMonster_Gunner> m_pOwner;

	State m_state = State::END_STATE;
	State m_preState = State::END_STATE;

public:
	State GetState() const { return m_state; };
};

END