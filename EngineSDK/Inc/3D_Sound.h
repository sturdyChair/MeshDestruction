//#pragma once
//#include "SoundManager.h"
//
//BEGIN(Engine)
//
//class C3D_Sound 
//{
//
//private:
//    C3D_Sound();
//public:
//    virtual ~C3D_Sound();
//
//public:
//    HRESULT Initialize(FMOD::System* pSystem, const wstring& strPath);
//
//    HRESULT Load(const wstring& _strPath);
//
//    // 일반 재생
//    void Play(_fvector vPos, _fvector vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f), bool _bLoop = false);
//
//    void Stop();
//
//    void SetPaused(bool _bPause = true);
//
//    // 볼륨 범위 (0 ~ 100)
//    void SetVolume(float _fVolume);
//    void SetRealVolume(float fVolume);
//
//    // 음악파일 위치 조정(0 ~ 100)
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
//    FMOD_VECTOR m_vCoord;
//    FMOD_VECTOR m_vVelocity;
//
//    int	m_iVolume;
//    float m_fRealVolume = 1.f;
//    float m_fSpeed = 1.f;
//
//public:
//    static shared_ptr<C3D_Sound> Create(FMOD::System* pSystem, const wstring& strPath);
//    void Free();
//
//    friend class CSoundManager;
//};
//
//
//END