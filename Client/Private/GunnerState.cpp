#include "GunnerState.h"

void CGunnerState::ChangeState(CGunnerState* pCurState, CGunnerState* pNextState)
{
	if (nullptr != pCurState)
	{
		pCurState->ExitState();
		Safe_Delete(pCurState);
	}

	pNextState->EnterState();
}
