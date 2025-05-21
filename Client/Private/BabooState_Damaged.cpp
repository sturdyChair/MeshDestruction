#include "BabooState_Damaged.h"
#include "GameInstance.h"
#include "Monster_Baboo.h"

#include "BabooState_Idle.h"

CBabooState_Damaged::CBabooState_Damaged(weak_ptr<class CMonster_Baboo> pBaboo, State preState, const DamageInfo& damageInfo, _bool isGroggy)
{
    m_state = State::DAMAGED;
    m_pOwner = pBaboo;
    m_preState = preState;

    m_damageInfo = damageInfo;

	m_isGroggy = isGroggy;
}

CBabooState* CBabooState_Damaged::StepState(_float fTimeDelta)
{
	_bool isAnimFinished = m_pOwner.lock()->IsAnimFinished();
	if (isAnimFinished && m_strCurAnim == "em0400_dmg-ex_500")
	{
		m_strCurAnim = "em0400_dmg-ex_501";
		m_pOwner.lock()->SetAnimHard(m_strCurAnim);
	}
	else if (isAnimFinished && m_strCurAnim == "em0400_dmg-ex_501")
	{
		m_strCurAnim = "em0400_dmg-ex_502";
		m_pOwner.lock()->SetAnimHard(m_strCurAnim);
	}
	else if (isAnimFinished)
		return new CBabooState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CBabooState_Damaged::ActiveState(_float fTimeDelta)
{
}

void CBabooState_Damaged::EnterState()
{
	if (m_isGroggy)
	{
		m_strCurAnim = "em0400_dmg-ex_500";
		m_pOwner.lock()->SetAnimHard(m_strCurAnim);
		return;
	}

	switch (m_damageInfo.eAttackType)
	{
	case EAttackType::Base:
	{
		m_pOwner.lock()->SetAnimHard("em0400_dmg-low_000");
		break;
	}
	case EAttackType::Hard:
	{
		m_pOwner.lock()->SetAnimHard("em0400_dmg-mid_000");
		//m_pOwner.lock()->SetAnimHard("em0400_dmg-mid_010");
		//m_pOwner.lock()->SetAnimHard("em0400_dmg-mid_020");
	}
	break;
	case EAttackType::Air:
	{
		if (m_pOwner.lock()->m_isAirState)
		{
			m_pOwner.lock()->SetAnimHard("em0400_dmg-air_200");
			//m_pOwner.lock()->SetAnimHard("em0400_dmg-air_210");
			//m_pOwner.lock()->SetAnimHard("em0400_dmg-air_220");
		}
		else
		{
			m_pOwner.lock()->SetAnimHard("em0400_dmg-air_200");
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

void CBabooState_Damaged::ExitState()
{
}
