#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

CTimer_Manager::~CTimer_Manager()
{
	Free();
}

_float CTimer_Manager::Get_TimeDelta(const wstring& strTimerTag)
{
	shared_ptr<CTimer> pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	shared_ptr<CTimer> pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

void CTimer_Manager::Compute_TimeDelta(const wstring& strTimerTag)
{
	shared_ptr<CTimer> pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return;

	pTimer->Update();
}

shared_ptr<CTimer> CTimer_Manager::Find_Timer(const wstring& strTimerTag)
{
	auto	iter = m_Timers.find(strTimerTag);
	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

shared_ptr<CTimer_Manager> CTimer_Manager::Create()
{
	MAKE_SHARED_ENABLER(CTimer_Manager)

	return make_shared<MakeSharedEnabler>();
}

void CTimer_Manager::Free()
{

}
