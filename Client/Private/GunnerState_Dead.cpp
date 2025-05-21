#include "GunnerState_Dead.h"
#include "Monster_Gunner.h"

CGunnerState_Dead::CGunnerState_Dead(weak_ptr<class CMonster_Gunner> pGunner, State preState, const DamageInfo& damageInfo)
{
	m_state = State::DEAD;
	m_pOwner = pGunner;
	m_preState = preState;

	m_damageInfo = damageInfo;
}

CGunnerState* CGunnerState_Dead::StepState(_float fTimeDelta)
{
	return nullptr;
}

void CGunnerState_Dead::ActiveState(_float fTimeDelta)
{
}

void CGunnerState_Dead::EnterState()
{
}

void CGunnerState_Dead::ExitState()
{
}
