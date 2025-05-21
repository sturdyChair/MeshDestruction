#include "SaberState_Damaged.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Monster_Saber.h"

#include "SaberState_Idle.h"

CSaberState_Damaged::CSaberState_Damaged(weak_ptr<class CMonster_Saber> pSaber, State preState, const DamageInfo& damageInfo)
{
	m_pOwner = pSaber;
	m_state = State::DAMAGED;
	m_preState = preState;
	m_damageInfo = damageInfo;
}

CSaberState* CSaberState_Damaged::StepState(_float fTimeDelta)
{
	if (m_pOwner.lock()->IsAnimFinished())
		return new CSaberState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CSaberState_Damaged::ActiveState(_float fTimeDelta)
{
}

void CSaberState_Damaged::EnterState()
{
	switch (m_damageInfo.eAttackType)
	{
	case EAttackType::Base:
	{
		m_pOwner.lock()->SetAnimHard("em0100_dmg-low_000");
		//if ()
		//	m_pOwner.lock()->SetAnimHard("em0100_dmg-low_010");
		//else if()
		//	m_pOwner.lock()->SetAnimHard("em0100_dmg-low_020");
		//else
		//	m_pOwner.lock()->SetAnimHard("em0100_dmg-low_000");
		break;
	}
	case EAttackType::Hard:
	{
		m_pOwner.lock()->SetAnimHard("em0100_dmg-mid_000");
		//m_pOwner.lock()->SetAnimHard("em0100_dmg-mid_010");
		//m_pOwner.lock()->SetAnimHard("em0100_dmg-mid_020");
	}
		break;
	case EAttackType::Air:
	{
		if (m_pOwner.lock()->m_isAirState)
		{
			m_pOwner.lock()->SetAnimHard("em0100_dmg-air_200");
			//m_pOwner.lock()->SetAnimHard("em0100_dmg-air_210");
			//m_pOwner.lock()->SetAnimHard("em0100_dmg-air_220");
		}
		else
		{
			m_pOwner.lock()->SetAnimHard("em0100_dmg-air_200");
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

void CSaberState_Damaged::ExitState()
{
}
