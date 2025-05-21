#include "BabooState_Move.h"
#include "GameInstance.h"
#include "Monster_Baboo.h"

#include "BabooState_Attack.h"

#include "Beat_Manager.h"


CBabooState_Move::CBabooState_Move(weak_ptr<class CMonster_Baboo> pBaboo, State preState)
{
    m_pOwner = pBaboo;
    m_state = State::MOVE;
    m_preState = preState;
}

CBabooState* CBabooState_Move::StepState(_float fTimeDelta)
{
    _bool isBeatAble = m_pOwner.lock()->IsBeatAble();
    if (!isBeatAble)
        return nullptr;

    if (m_pOwner.lock()->m_fTimer_Attack <= 0.f)
        return new CBabooState_Attack(m_pOwner, m_state);

	return nullptr;
}

void CBabooState_Move::ActiveState(_float fTimeDelta)
{
    _uint iBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    if (m_iCurBeat != iBeat)
    {
        m_iCurBeat = iBeat;
        m_iBeatCnt++;
    }

    if (m_iBeatCnt >= m_iTargetBeat)
        SelectMoveAnim();


    if (m_isLookStrong)
    {
        _vector vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);
    }
    else
    {
        _float fSpeedRotation = 1.5f;
        auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
    }

    if (m_strCurAnim == "em0400_mvnt-walk-st_010")
    {
        m_pOwner.lock()->MoveStraight(4.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_001")
    {
        m_pOwner.lock()->MoveStraight(-2.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_012")
    {
        m_pOwner.lock()->MoveRight(-2.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_013")
    {
        m_pOwner.lock()->MoveRight(2.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_006")
    {
        _float fSpeed = sqrtf(2.f);
        m_pOwner.lock()->MoveStraight(-fSpeed);
        m_pOwner.lock()->MoveRight(-fSpeed);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_007")
    {
        _float fSpeed = sqrtf(2.f);
        m_pOwner.lock()->MoveStraight(-fSpeed);
        m_pOwner.lock()->MoveRight(fSpeed);
    }
}

void CBabooState_Move::EnterState()
{
    SelectMoveAnim();
}

void CBabooState_Move::ExitState()
{
}

void CBabooState_Move::SelectMoveAnim()
{
    m_iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    if (CBeat_Manager::Get_Instance()->Get_Timer() > CBeat_Manager::Get_Instance()->Get_Beat() * 0.9f)
        m_iBeatCnt = -1;
    else
        m_iBeatCnt = 0;

    _float fDistance = m_pOwner.lock()->m_fDistance;
    if (fDistance > m_pOwner.lock()->m_fDistance_Far_C)
    {
        m_iTargetBeat = 2;
        m_isLookStrong = false;

        m_strCurAnim = "em0400_mvnt-walk-st_010";
        m_pOwner.lock()->SetAnim(m_strCurAnim, true);
    }
    else if (fDistance < m_pOwner.lock()->m_fDistance_Near_C)
    {
        m_iTargetBeat = 1;
        m_isLookStrong = true;

        switch (GAMEINSTANCE->Random_Int(0, 2))
        {
        case 0:
            m_strCurAnim = "em0400_mvnt-walk-st_001";
            break;
        case 1:
            m_strCurAnim = "em0400_mvnt-walk-st_006";
            break;
        case 2:
            m_strCurAnim = "em0400_mvnt-walk-st_007";
            break;
        }
        m_pOwner.lock()->SetAnim(m_strCurAnim, true);
    }
    else
    {
        m_iTargetBeat = 3;
        m_isLookStrong = true;

        switch (GAMEINSTANCE->Random_Int(0, 1))
        {
        case 0:
            m_strCurAnim = "em0400_mvnt-walk-st_012";
            break;
        case 1:
            m_strCurAnim = "em0400_mvnt-walk-st_013";
            break;
        }
        m_pOwner.lock()->SetAnim(m_strCurAnim, true);
    }
}
