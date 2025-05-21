#include "GunnerState_Move.h"
#include "Monster_Gunner.h"

CGunnerState_Move::CGunnerState_Move(weak_ptr<class CMonster_Gunner> pGunner, State preState)
{
	m_state = State::MOVE;
	m_pOwner = pGunner;
	m_preState = preState;
}

CGunnerState* CGunnerState_Move::StepState(_float fTimeDelta)
{
	return nullptr;
}

void CGunnerState_Move::ActiveState(_float fTimeDelta)
{
}

void CGunnerState_Move::EnterState()
{
}

void CGunnerState_Move::ExitState()
{
}
