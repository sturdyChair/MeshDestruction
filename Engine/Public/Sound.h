//#pragma once
//#include "SoundManager.h"
//
//BEGIN(Engine)
//
//class CSound
//{
//
//private:
//    CSound();
//public:
//    virtual ~CSound();
//
//public:
//    HRESULT Initialize(FMOD::System* pSystem, const wstring& strPath);
//
//    HRESULT Load(const wstring& _strPath);
//
//    // �Ϲ� ���
//    void Play(bool _bLoop = false);
//
//    // BGM ���� ���
//    void PlayToBGM(bool _bLoop = true);
//
//    void Stop();
//
//    void SetPaused(bool _bPause = true);
//
//    // ���� ���� (0 ~ 100)
//    void SetVolume(float _fVolume);
//    void SetRealVolume(float fVolume);
//
//    // �������� ��ġ ����(0 ~ 100)
//    void SetPosition(float _fPosition);
//
//    void SetSpeed(_float fSpeed);
//
//    bool IsPlaying();
//
//private:
//    int GetDecibel(float _fVolume);
//
//private:
//    FMOD::Sound* m_pSound;
//    FMOD::Channel* m_pChannel;
//    FMOD_RESULT m_FmodR;
//    FMOD::System* m_pSystem = nullptr;
//
//    int	m_iVolume;
//    float m_fRealVolume = 1.f;
//    float m_fSpeed = 1.f;
//
//public:
//    static shared_ptr<CSound> Create(FMOD::System* pSystem, const wstring& strPath);
//    void Free();
//
//    friend CSoundManager;
//};
//
//
//END