#include "SaberState_Attack.h"
#include "GameInstance.h"
#include "Monster_Saber.h"

#include "SaberState_Move.h"

CSaberState_Attack::CSaberState_Attack(weak_ptr<class CMonster_Saber> pSaber, State preState)
{
	m_pOwner = pSaber;
	m_preState = preState;
	m_state = State::ATTACK;
}

CSaberState* CSaberState_Attack::StepState(_float fTimeDelta)
{
	if (m_pOwner.lock()->IsAnimFinished())
		return new CSaberState_Move(m_pOwner, m_state);

	return nullptr;
}

void CSaberState_Attack::ActiveState(_float fTimeDelta)
{
}

void CSaberState_Attack::EnterState()
{
	if (GAMEINSTANCE->Random_Bool())
		m_pOwner.lock()->SetAnim("em0100_sword-a_000");
	else
		m_pOwner.lock()->SetAnim("em0100_sword-a_001");
}

void CSaberState_Attack::ExitState()
{
}
