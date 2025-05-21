#include "SaberState_Dead.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Monster_Saber.h"

CSaberState_Dead::CSaberState_Dead(weak_ptr<class CMonster_Saber> pSaber, State preState, const DamageInfo& damageInfo)
{
	m_pOwner = pSaber;
	m_state = State::DEAD;
	m_preState = preState;
	m_damageInfo = damageInfo;
}

CSaberState* CSaberState_Dead::StepState(_float fTimeDelta)
{
	return nullptr;
}

void CSaberState_Dead::ActiveState(_float fTimeDelta)
{
	// TODO: Dissolve or dead set
}

void CSaberState_Dead::EnterState()
{
	m_pOwner.lock()->SetAnim("em0100_dmg-die_100");
}

void CSaberState_Dead::ExitState()
{
}
