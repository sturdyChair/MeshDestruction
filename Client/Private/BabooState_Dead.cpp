#include "BabooState_Dead.h"
#include "Monster_Baboo.h"

CBabooState_Dead::CBabooState_Dead(weak_ptr<class CMonster_Baboo> pBaboo, State preState, const DamageInfo& damageInfo)
{
	m_state = State::DEAD;
	m_pOwner = pBaboo;
	m_preState = preState;

	m_damageInfo = damageInfo;
}

CBabooState* CBabooState_Dead::StepState(_float fTimeDelta)
{
	return nullptr;
}

void CBabooState_Dead::ActiveState(_float fTimeDelta)
{
}

void CBabooState_Dead::EnterState()
{
}

void CBabooState_Dead::ExitState()
{
}
