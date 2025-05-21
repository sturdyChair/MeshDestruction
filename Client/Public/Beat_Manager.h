#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CTexture;
END

BEGIN(Client)

class CBeat_Manager
{
	DECLARE_SINGLETON(CBeat_Manager)
private:
	CBeat_Manager();
public:
	virtual ~CBeat_Manager();

public:
	void Tick(_float fTimeDelta);

	void Set_Beat(_float fBeat) { m_fBeat = fBeat; }
	void Reset()
	{
		m_fTimer = 0.f; m_iBeatCount = 0;
	}

	_float Get_Beat() const { return m_fBeat; }
	_uint Get_BeatCount() const { return m_iBeatCount; }
	_float Get_Timer() const { return m_fTimer; }

	void Initialize();
	void Kill_Instance();

	void Queue_Sequence(const _wstring& strSeqTag)
	{
		m_bSequenceStoped = false;
		m_bSequenceChange = true;
		m_strNextSequenceTag = strSeqTag;
	}
	void Stop_Sequence();

	void Start_Sequence();
	void Proceed();
	_float Get_N_Beats(_uint iN) { return m_fNextBeats[iN]; }

	_bool Beat_Move(_float fRate);

private:
	_float m_fBeat = 0.f;
	_float m_fTimer = 0.f;
	_uint  m_iBeatCount = 0;
	_float m_fNextBeats[8]{};

	_bool m_bSequenceStoped = false;
	bool   m_bSequenceChange = false;
	_wstring m_strNextSequenceTag;
	//BGM_SEQUENCE* m_pCurrSequence = nullptr;
	//map<_wstring, BGM_SEQUENCE*> m_mapBGM;
	//vector<BGM_SEQUENCE> m_vecBGMSequence;
	//CGameInstance* m_pGameInstance = nullptr;

	_uint m_iBeatMoved = 0;
	

public:
	void Free();
};

END