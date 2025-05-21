#include "GunnerState_Attack.h"
#include "Monster_Gunner.h"

#include "GunnerState_Idle.h"

CGunnerState_Attack::CGunnerState_Attack(weak_ptr<class CMonster_Gunner> pGunner, State preState)
{
    m_pOwner = pGunner;
    m_state = State::ATTACK;
    m_preState = preState;
}

CGunnerState* CGunnerState_Attack::StepState(_float fTimeDelta)
{
    _bool isAnimFinished = m_pOwner.lock()->IsAnimFinished();
    if (isAnimFinished && m_strCurAnim == "em0200_atk-gun_002")
        return new CGunnerState_Idle(m_pOwner, m_state);

    return nullptr;
}

void CGunnerState_Attack::ActiveState(_float fTimeDelta)
{
    if (m_strCurAnim != "em0200_atk-gun_002")
    {
        auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * m_fSpeedRotation_C);
    }

    _bool isAnimFinished = m_pOwner.lock()->IsAnimFinished();
    if (!isAnimFinished)
        return;

    if (m_strCurAnim == "em0200_atk-gun_000")
    {
        m_strCurAnim = "em0200_atk-gun_001";
        m_pOwner.lock()->SetAnim(m_strCurAnim);
    }
    else if (m_strCurAnim == "em0200_atk-gun_001")
    {
        m_strCurAnim = "em0200_atk-gun_002";
        m_pOwner.lock()->SetAnim(m_strCurAnim);
    }
}

void CGunnerState_Attack::EnterState()
{
    // Reset Attack Timer
    m_pOwner.lock()->m_fTimer_Attack = m_pOwner.lock()->m_fTimer_Attack_C;

    m_strCurAnim = "em0200_atk-gun_000";
    m_pOwner.lock()->SetAnim(m_strCurAnim);
}

void CGunnerState_Attack::ExitState()
{
}
