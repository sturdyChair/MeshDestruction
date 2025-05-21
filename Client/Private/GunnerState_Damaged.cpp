#include "GunnerState_Damaged.h"
#include "Monster_Gunner.h"

#include "GunnerState_Idle.h"

CGunnerState_Damaged::CGunnerState_Damaged(weak_ptr<class CMonster_Gunner> pGunner, State preState, const DamageInfo& damageInfo)
{
    m_state = State::DAMAGED;
    m_pOwner = pGunner;
    m_preState = preState;

    m_damageInfo = damageInfo;
}

CGunnerState* CGunnerState_Damaged::StepState(_float fTimeDelta)
{
	if (m_pOwner.lock()->IsAnimFinished())
		return new CGunnerState_Idle(m_pOwner, m_state);

    return nullptr;
}

void CGunnerState_Damaged::ActiveState(_float fTimeDelta)
{
}

void CGunnerState_Damaged::EnterState()
{
	switch (m_damageInfo.eAttackType)
	{
	case EAttackType::Base:
	{
		m_pOwner.lock()->SetAnimHard("em0200_dmg-low_010");
		//if ()
		//	m_pOwner.lock()->SetAnimHard("em0200_dmg-low_010");
		//else if()
		//	m_pOwner.lock()->SetAnimHard("em0200_dmg-low_020");
		break;
	}
	case EAttackType::Hard:
	{
		//m_pOwner.lock()->SetAnimHard("em0200_dmg-mid_010");
		//m_pOwner.lock()->SetAnimHard("em0200_dmg-mid_020");
	}
	break;
	case EAttackType::Air:
	{
		if (m_pOwner.lock()->m_isAirState)
		{
			m_pOwner.lock()->SetAnimHard("em0200_dmg-air_200");
			//m_pOwner.lock()->SetAnimHard("em0200_dmg-air_210");
			//m_pOwner.lock()->SetAnimHard("em0200_dmg-air_220");
		}
		else
		{
			m_pOwner.lock()->SetAnimHard("em0200_dmg-air_200");
			// Go to Air!!
		}
	}
	break;
	case EAttackType::Special:
		break;
	}

	// Hardly look player
	_vector vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
	m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);
}

void CGunnerState_Damaged::ExitState()
{
}
