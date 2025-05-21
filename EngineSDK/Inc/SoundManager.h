//#pragma once
//#include <fmod.hpp>
//#include <fmod_errors.h>
//
//#ifdef _DEBUG
//#pragma comment(lib, "fmodL_vc.lib")// _DEBUG
//#else
//#pragma comment(lib, "fmod_vc.lib")
//#endif
//
//#include "Engine_Defines.h"
//
//BEGIN(Engine)
//class CSound;
//class C3D_Sound;
//class CSoundManager 
//{
//private:
//    CSoundManager();
//public:
//    virtual ~CSoundManager();
//
//public:
//    HRESULT Initialize(void);
//    FMOD::System* GetFmodSystem() { return m_pFmodSystem; }
//    void Update();
//    void RegisterToBGM(shared_ptr<CSound> _pSound);
//    void Set_BGM_Tag(const wstring& strTag) { m_strBGMTag = strTag; }
//    wstring Get_BGM_Tag() const { return m_strBGMTag; }
//    void Stop_BGM();
//    void Start_BGM();
//    bool IsBGMExist() { return nullptr != m_pBGM; }
//    bool IsBGMPlaying();
//
//    HRESULT Add_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath);
//    HRESULT Add_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath);
//
//    void Set_Master_Volume(float fVolume);
//    void Set_Channel_Volume(SOUND_CHANNEL eChannel, float fVolume);
//
//    void Play_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bLoop = false);
//    void Play_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, _fvector vPos, _fvector vVel = XMVectorSet(0.f, 0.f, 0.f, 0.f), bool bLoop = false);
//
//    void Pause_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing = true);
//    void Pause_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing = true);
//
//    void Stop_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag);
//    void Stop_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag);
//
//    void Set_Sound_Volume(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fVolume);
//
//    void Set_Master_Speed(float fSpeed);
//    void Set_Channel_Speed(SOUND_CHANNEL eChannel, float fSpeed);
//    void Set_Sound_Speed(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fSpeed);
//
//    void Set_BGM_Position(_float fPosition);
//
//    shared_ptr<CSound> Find_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag);
//    shared_ptr<C3D_Sound> Find_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag);
//
//    void Set_Listener(shared_ptr<class CGameObject> pListener);
//
//private:
//    shared_ptr<CSound> m_pBGM;
//    wstring m_strBGMTag;
//    FMOD::System* m_pFmodSystem;
//    map<wstring, shared_ptr<CSound>> m_Sounds[(_uint)SOUND_CHANNEL::CHANNEL_END];
//    map<wstring, shared_ptr<C3D_Sound>> m_3D_Sounds[(_uint)SOUND_CHANNEL::CHANNEL_END];
//    float m_fMasterVolume = 0.f;
//    float m_fChannelVolume[(_uint)SOUND_CHANNEL::CHANNEL_END] = {};
//
//    shared_ptr<class CGameObject> m_pListener = nullptr;
//    _float4 m_vPrevPosition;
//
//public:
//    static shared_ptr<CSoundManager> Create();
//    void Free();
//};
//END