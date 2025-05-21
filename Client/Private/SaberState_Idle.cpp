#include "SaberState_Idle.h"
#include "GameInstance.h"
#include "Monster_Saber.h"

#include "SaberState_Move.h"

CSaberState_Idle::CSaberState_Idle(weak_ptr<class CMonster_Saber> pSaber, State preState)
{
    m_pOwner = pSaber;
    m_state = State::IDLE;
    m_preState = preState;
}

CSaberState* CSaberState_Idle::StepState(_float fTimeDelta)
{
    if (m_pOwner.lock()->IsAnimFinished())
        return new CSaberState_Move(m_pOwner, m_state);

    return nullptr;
}

void CSaberState_Idle::ActiveState(_float fTimeDelta)
{
    // Player 바라보도록 설정하기
    _vector vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
    m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);
}

void CSaberState_Idle::EnterState()
{
    switch (GAMEINSTANCE->Random_Int(0, 2))
    {
    case 0:
        m_pOwner.lock()->SetAnim("idle-accent_0");
        break;
    case 1:
        m_pOwner.lock()->SetAnim("idle-accent_1");
        break;
    case 2:
        m_pOwner.lock()->SetAnim("idle-accent_2");
        break;
    }

}

void CSaberState_Idle::ExitState()
{
}
