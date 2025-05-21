#include "BabooState_Idle.h"
#include "GameInstance.h"
#include "Monster_Baboo.h"

#include "BabooState_Move.h"
#include "BabooState_Attack.h"

CBabooState_Idle::CBabooState_Idle(weak_ptr<class CMonster_Baboo> pBaboo, State preState)
{
    m_pOwner = pBaboo;
    m_state = State::IDLE;
    m_preState = preState;
}

CBabooState* CBabooState_Idle::StepState(_float fTimeDelta)
{
    _bool isBeatAble = m_pOwner.lock()->IsBeatAble();
    if (!isBeatAble)
        return nullptr;

    if (m_pOwner.lock()->m_fTimer_Attack <= 0.f)
        return new CBabooState_Attack(m_pOwner, m_state);
    if (m_pOwner.lock()->IsNeedToMove())
        return new CBabooState_Move(m_pOwner, m_state);

	return nullptr;
}

void CBabooState_Idle::ActiveState(_float fTimeDelta)
{
}

void CBabooState_Idle::EnterState()
{
    m_pOwner.lock()->SetAnim("em0400_idle_000", true);
}

void CBabooState_Idle::ExitState()
{
}
