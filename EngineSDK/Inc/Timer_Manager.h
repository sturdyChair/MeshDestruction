#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CTimer_Manager
{
private:
	CTimer_Manager();

public:
	~CTimer_Manager();

public:
	_float	Get_TimeDelta(const wstring& strTimerTag);

public:
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);

	void Set_TimeMultiplier(_float fTimeMultiplier) {
		m_fTimeMultiplier = fTimeMultiplier;
		if (m_fTimeMultiplier > m_fTimeMultiplier_Max)
			m_fTimeMultiplier = m_fTimeMultiplier_Max;

		if (m_fTimeMultiplier < m_fTimeMultiplier_Min)
			m_fTimeMultiplier = m_fTimeMultiplier_Min;
	}
	_float Get_TimeMultiplier() { return m_fTimeMultiplier; }

private:
	map<const wstring, shared_ptr<class CTimer>>			m_Timers;

	_float m_fTimeMultiplier = 1.f;
	_float m_fTimeMultiplier_Min = 0.f;
	_float m_fTimeMultiplier_Max = 1.f;
private:
	shared_ptr<class CTimer> Find_Timer(const wstring& strTimerTag);

public:
	static shared_ptr<CTimer_Manager> Create();
	void Free();
};

END