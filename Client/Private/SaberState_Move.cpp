#include "SaberState_Move.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Monster_Saber.h"

#include "SaberState_Idle.h"
#include "SaberState_Attack.h"

CSaberState_Move::CSaberState_Move(weak_ptr<CMonster_Saber> pSaber, State preState)
{
    m_pOwner = pSaber;
	m_state = State::MOVE;
	m_preState = preState;

	m_fDistance_Close = 5.f;
	m_fDistance_Too_Close = 3.f;
}

CSaberState* CSaberState_Move::StepState(_float fTimeDelta)
{
	m_fCurTimer -= fTimeDelta;
	if (m_fCurTimer < 0.f && m_pOwner.lock()->IsBeatAble())
		return new CSaberState_Attack(m_pOwner, m_state);

    return nullptr;
}

void CSaberState_Move::ActiveState(_float fTimeDelta)
{
	SelectMoveAnim();

	if (m_pOwner.lock()->IsAnimFinished())
		m_isActivingShort = false;

	// Set looking player
	_vector vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
	m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);
}

void CSaberState_Move::EnterState()
{
	m_fCurTimer = GAMEINSTANCE->Random_Float(3.f, 6.f);

	/*
	// 초기에 목표 위치로 향한다
	_float fRadianInit = GAMEINSTANCE->Random_Float(0.f, 6.283f);
	XMStoreFloat4(&m_vTargetPos,
		PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position()
		+ XMVectorSet(sinf(fRadianInit), 0.f, cosf(fRadianInit), 0.f) * 20.f);
	*/
}

void CSaberState_Move::ExitState()
{
}

void CSaberState_Move::SelectMoveAnim()
{
	// Distance Check
	if (m_pOwner.lock()->m_fDistance <= m_fDistance_Too_Close)
	{
		m_pOwner.lock()->SetAnimHard("em0100_mvnt-walk-st_091", true);
		m_isActivingShort = false;
	}
	else if (m_pOwner.lock()->m_fDistance <= m_fDistance_Close)
	{
		if (!m_isActivingShort)
		{
			m_isActivingShort = true;

			switch (GAMEINSTANCE->Random_Int(0, 5))
			{
			case 0:
				m_pOwner.lock()->SetAnim("em0100_mvnt-walk-st_090");
				break;
			case 1:
				m_pOwner.lock()->SetAnim("em0100_mvnt-walk-st_090");
				break;
			case 2:
				m_pOwner.lock()->SetAnim("em0100_mvnt-walk-st_092");
				break;
			case 3:
				m_pOwner.lock()->SetAnim("em0100_mvnt-walk-st_093");
				break;
			case 4:
				m_pOwner.lock()->SetAnim("em0100_mvnt-walk-st_002");
				break;
			case 5:
				m_pOwner.lock()->SetAnim("em0100_mvnt-walk-st_003");
				break;
			}
		}
	}
	else
	{
		if (!m_isActivingShort)
		{
			m_pOwner.lock()->SetAnim("em0100_mvnt-walk-st_000", true);
		}
	}

}
