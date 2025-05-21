#include "SaberState.h"

void CSaberState::ChangeState(CSaberState* pCurState, CSaberState* pNextState)
{
	if (nullptr != pCurState)
	{
		pCurState->ExitState();
		Safe_Delete(pCurState);
	}

	pNextState->EnterState();
}
