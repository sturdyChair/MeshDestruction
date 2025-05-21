#include "Beat_Manager.h"
#include "GameInstance.h"
#include "Texture.h"

IMPLEMENT_SINGLETON(CBeat_Manager)

CBeat_Manager::CBeat_Manager()
{
}

CBeat_Manager::~CBeat_Manager()
{
	Free();
}

void CBeat_Manager::Tick(_float fTimeDelta)
{
	m_fTimer += fTimeDelta;
	for (_uint i = 0; i < 8; ++i)
	{
		m_fNextBeats[i] -= fTimeDelta;
		if (m_fNextBeats[i] <= 0.f)
		{
			m_fNextBeats[i] += m_fBeat * 8.f;
			//if (i % 2)
			//{
			//	m_eNextBeatTypes[i] = BM_DEFAULT;
			//}
			//else
			//{
			//	m_eNextBeatTypes[i] = BM_SMALL;
			//}
		}
	}
	if (m_fTimer >= m_fBeat)
	{
		++m_iBeatCount;
		m_fTimer -= m_fBeat;

		if (m_bSequenceChange)
		{
			Start_Sequence();
		}
		//if (m_pCurrSequence)
		//{
		//	if (m_iBeatCount >= m_pCurrSequence->iBeat)
		//	{
		//		Proceed();
		//	}
		//}

	}
}

void CBeat_Manager::Initialize()
{
}

void CBeat_Manager::Kill_Instance()
{
	m_pInstance->Free();
	m_pInstance->Destroy_Instance();
}

void CBeat_Manager::Stop_Sequence()
{
	m_bSequenceStoped = true;
	//if (m_pCurrSequence)
	//{
	//	m_pGameInstance->Stop_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);
	//}
}

void CBeat_Manager::Start_Sequence()
{
	m_bSequenceChange = false;
	//if (m_pCurrSequence)
	//{
	//	m_pGameInstance->Stop_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);
	//}
	//m_pCurrSequence = m_mapBGM[m_strNextSequenceTag];
	//m_pGameInstance->Play_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);
	m_iBeatCount = 0;
	for (_uint i = 0; i < 8; ++i)
	{
		m_fNextBeats[i] = (_float)i * m_fBeat;
	}
	m_fTimer = 0.f;
}

void CBeat_Manager::Proceed()
{
	if (m_bSequenceStoped)
	{
		return;
	}
	//m_pCurrSequence = m_pCurrSequence->pNext;
	//if (m_pCurrSequence)
	//{
	//	m_pGameInstance->Play_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);
	//}
	m_iBeatCount = 0;
	for (_uint i = 0; i < 8; ++i)
	{
		m_fNextBeats[i] = (_float)i * m_fBeat;
	}
	m_fTimer = 0.f;
}

_bool CBeat_Manager::Beat_Move(_float fRate)
{
	if (m_iBeatCount == m_iBeatMoved)
		return false;
	_float fDiff = min(m_fBeat - m_fTimer, m_fTimer);
	
	if (abs(fDiff) > m_fBeat * fRate)
	{
		return false;
	}
	
	if (m_fTimer > m_fBeat * 0.5f)
	{
		m_iBeatMoved = m_iBeatCount + 1;
	}
	else
	{
		m_iBeatMoved = m_iBeatCount;
	}

	return true;
}

//void CBeat_Manager::Set_N_BeatType(_uint iN, BM_TYPE eType)
//{
//	_float fLeast = 10.f;
//	_uint iIdx = 0;
//	for (_uint i = 0; i < 8; ++i)
//	{
//		if (fLeast > m_fNextBeats[i])
//		{
//			fLeast = m_fNextBeats[i];
//			iIdx = i;
//		}
//	}
//
//	_uint iNext = iIdx + iN;
//	iNext %= 8;
//	m_eNextBeatTypes[iNext] = eType;
//
//}



void CBeat_Manager::Free()
{
}
