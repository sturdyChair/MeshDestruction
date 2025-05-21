#include "Timer.h"

CTimer::CTimer()
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

CTimer::~CTimer()
{
	Free();
}

HRESULT CTimer::Initialize()
{
	//QueryPerformanceCounter : 메인보드가 지닌 고해상도 타이머의 누적 값을 얻어오는 함수

	QueryPerformanceCounter(&m_CurTime);		// 1000
	QueryPerformanceCounter(&m_OldTime);		// 1024
	QueryPerformanceCounter(&m_OriginTime);		// 1030
	// QueryPerformanceFrequency : 고해상도 타이머의 주파수를 얻어오는 함수, 여기서 주파수는 cpu의 초당 클럭수를 의미
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void CTimer::Update()
{
	QueryPerformanceCounter(&m_CurTime);	// 1100

	if (m_CurTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurTime;
	}

	m_fTimeDelta = float(m_CurTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	m_OldTime = m_CurTime;
}

shared_ptr<CTimer> CTimer::Create()
{
	MAKE_SHARED_ENABLER(CTimer);
	shared_ptr<CTimer> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTimer"));
		assert(false);
	}

	return pInstance;
}


void CTimer::Free()
{
}