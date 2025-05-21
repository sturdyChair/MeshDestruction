#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CTimer
{
private:
	CTimer();

public:
	~CTimer();

public:
	float Get_TimeDelta() {	return m_fTimeDelta; }

public:
	HRESULT			Initialize();
	void			Update();

private:
	LARGE_INTEGER			m_CurTime;
	LARGE_INTEGER			m_OldTime;
	LARGE_INTEGER			m_OriginTime;
	LARGE_INTEGER			m_CpuTick;

	float					m_fTimeDelta;

public:
	static shared_ptr<CTimer> Create();
	void Free();
};


END