#include "BabooState.h"

void CBabooState::ChangeState(CBabooState* pCurState, CBabooState* pNextState)
{
	if (nullptr != pCurState)
	{
		pCurState->ExitState();
		Safe_Delete(pCurState);
	}

	pNextState->EnterState();
}
