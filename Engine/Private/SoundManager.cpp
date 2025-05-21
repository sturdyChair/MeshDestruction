//#include "SoundManager.h"
//#include "Sound.h"
//#include "3D_Sound.h"
//#include "GameObject.h"
//#include  "Transform.h"
//CSoundManager::CSoundManager() :
//    m_pBGM(nullptr),
//    m_pFmodSystem(nullptr)
//{
//}
//
//CSoundManager::~CSoundManager()
//{
//    Free();
//}
//
//HRESULT CSoundManager::Initialize(void)
//{
//    FMOD_RESULT fmResult = FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG, FMOD_DEBUG_MODE_FILE, 0, "err.txt");
//
//    fmResult = FMOD::System_Create(&m_pFmodSystem);
//    if (fmResult != FMOD_OK)
//    {
//        printf("FMOD error! (%d) %s\n", fmResult, FMOD_ErrorString(fmResult));
//        return E_FAIL;
//    }
//    fmResult = m_pFmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);
//    if (fmResult != FMOD_OK)
//    {
//        printf("FMOD error! (%d) %s\n", fmResult, FMOD_ErrorString(fmResult));
//        return E_FAIL;
//    }
//    m_fMasterVolume = 100.f;
//    for (_uint i = 0; i < (_uint)SOUND_CHANNEL::CHANNEL_END; ++i)
//    {
//        m_fChannelVolume[i] = 100.f;
//    }
//    m_vPrevPosition = _float4{ 0.f,0.f,0.f,1.f };
//
//    return S_OK;
//}
//
//void CSoundManager::Update()
//{
//    if (m_pListener)
//    {
//        shared_ptr<CTransform> pTransform = m_pListener->Get_Transform();
//        auto vPos = pTransform->Get_Position();
//        _vector vPrevPos = XMLoadFloat4(&m_vPrevPosition);
//        _float3 vDisp;
//        XMStoreFloat3(&vDisp, vPos - vPrevPos);
//        _float3 vPosF3;
//        XMStoreFloat3(&vPosF3, vPos);
//        FMOD_VECTOR fvPos{ vPosF3.x,vPosF3.y,vPosF3.z };
//        FMOD_VECTOR fvDisp{ vDisp.x,vDisp.y,vDisp.z };
//        _float3 vLook;
//        XMStoreFloat3(&vLook, pTransform->Get_State(CTransform::STATE_LOOK));
//        _float3 vUp;
//        XMStoreFloat3(&vUp, pTransform->Get_State(CTransform::STATE_UP));
//
//
//        FMOD_VECTOR fvLook{ vLook.x,vLook.y,vLook.z };
//        FMOD_VECTOR fvUp{ vUp.x,vUp.y,vUp.z };
//        m_pFmodSystem->set3DListenerAttributes(0, &fvPos, &fvDisp, &fvLook, &fvUp);
//    }
//
//    m_pFmodSystem->update();
//}
//
//void CSoundManager::RegisterToBGM(shared_ptr<CSound> _pSound)
//{
//    if (m_pBGM != nullptr)
//    {
//        m_pBGM->Stop();
//        //m_pBGM->m_pSound->setDefaults(48000, 0);
//    }
//
//    m_pBGM = _pSound;
//    m_pBGM->PlayToBGM();
//}
//
//void CSoundManager::Stop_BGM()
//{
//    if (m_pBGM)
//    {
//        m_pBGM->Stop();
//    }
//}
//
//void CSoundManager::Start_BGM()
//{
//    if (m_pBGM)
//    {
//        m_pBGM->PlayToBGM();
//    }
//}
//
//bool CSoundManager::IsBGMPlaying()
//{
//    if (m_pBGM)
//    {
//        return m_pBGM->IsPlaying();
//    }
//    return false;
//}
//
//HRESULT CSoundManager::Add_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath)
//{
//    if (Find_Sound(eChannel, strSoundTag))
//    {
//
//        return E_FAIL;
//    }
//
//    shared_ptr<CSound> pSound = CSound::Create(m_pFmodSystem, strSoundPath);
//    if (!pSound)
//    {
//        return E_FAIL;
//    }
//
//
//    m_Sounds[(_uint)eChannel].insert({ strSoundTag, pSound });
//
//    return S_OK;
//}
//
//HRESULT CSoundManager::Add_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath)
//{
//    if (Find_Sound_3D(eChannel, strSoundTag))
//    {
//
//        return E_FAIL;
//    }
//
//    shared_ptr<C3D_Sound> pSound = C3D_Sound::Create(m_pFmodSystem, strSoundPath);
//    if (!pSound)
//    {
//        return E_FAIL;
//    }
//
//    m_3D_Sounds[(_uint)eChannel].insert({ strSoundTag, pSound });
//
//    return S_OK;
//}
//
//void CSoundManager::Set_Master_Volume(float fVolume)
//{
//    m_fMasterVolume = fVolume;
//    for (_uint i = 0; i < (_uint)SOUND_CHANNEL::CHANNEL_END; ++i)
//    {
//        for (auto& pSound : m_Sounds[i])
//        {
//            pSound.second->SetVolume(m_fMasterVolume * 0.01f * m_fChannelVolume[i] * 0.01f * pSound.second->m_fRealVolume * 0.01f);
//        }
//        for (auto& pSound : m_3D_Sounds[i])
//        {
//            pSound.second->SetVolume(m_fMasterVolume * 0.01f * m_fChannelVolume[i] * 0.01f * pSound.second->m_fRealVolume * 0.01f);
//        }
//    }
//}
//
//void CSoundManager::Set_Channel_Volume(SOUND_CHANNEL eChannel, float fVolume)
//{
//    m_fChannelVolume[(_uint)eChannel] = fVolume;
//    for (_uint i = 0; i < (_uint)SOUND_CHANNEL::CHANNEL_END; ++i)
//    {
//        for (auto& pSound : m_Sounds[i])
//        {
//            pSound.second->SetVolume(m_fMasterVolume * 0.01f * m_fChannelVolume[i] * 0.01f * pSound.second->m_fRealVolume * 0.01f);
//        }
//        for (auto& pSound : m_3D_Sounds[i])
//        {
//            pSound.second->SetVolume(m_fMasterVolume * 0.01f * m_fChannelVolume[i] * 0.01f * pSound.second->m_fRealVolume * 0.01f);
//        }
//    }
//}
//
//void CSoundManager::Play_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bLoop)
//{
//    shared_ptr<CSound> pSound = Find_Sound(eChannel, strSoundTag);
//    if (pSound)
//    {
//        //pSound->Stop();
//        pSound->Play(bLoop);
//    }
//}
//
//void CSoundManager::Play_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, _fvector vPos, _fvector vVel, bool bLoop)
//{
//    shared_ptr<C3D_Sound> pSound = Find_Sound_3D(eChannel, strSoundTag);
//    if (pSound)
//    {
//        //pSound->Stop();
//        pSound->Play(vPos, vVel, bLoop);
//    }
//}
//
//void CSoundManager::Pause_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing)
//{
//    shared_ptr<CSound> pSound = Find_Sound(eChannel, strSoundTag);
//    if (pSound)
//    {
//        pSound->SetPaused(bPausing);
//    }
//}
//
//void CSoundManager::Pause_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing)
//{
//    shared_ptr<C3D_Sound> pSound = Find_Sound_3D(eChannel, strSoundTag);
//    if (pSound)
//    {
//        pSound->SetPaused(bPausing);
//    }
//}
//
//void CSoundManager::Stop_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//    shared_ptr<CSound> pSound = Find_Sound(eChannel, strSoundTag);
//    if (pSound)
//    {
//        pSound->Stop();
//    }
//}
//
//void CSoundManager::Stop_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//    shared_ptr<C3D_Sound> pSound = Find_Sound_3D(eChannel, strSoundTag);
//    if (pSound)
//    {
//        pSound->Stop();
//    }
//}
//
//void CSoundManager::Set_Sound_Volume(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fVolume)
//{
//    shared_ptr<CSound> pSound = Find_Sound(eChannel, strSoundTag);
//    if (pSound)
//    {
//        pSound->m_fRealVolume = fVolume;
//    }
//}
//
//void CSoundManager::Set_Master_Speed(float fSpeed)
//{
//    for (auto& Sounds : m_Sounds)
//    {
//        for (auto& Sound : Sounds)
//        {
//            Sound.second->SetSpeed(fSpeed);
//        }
//    }
//    for (auto& Sounds : m_3D_Sounds)
//    {
//        for (auto& Sound : Sounds)
//        {
//            Sound.second->SetSpeed(fSpeed);
//        }
//    }
//}
//
//void CSoundManager::Set_Channel_Speed(SOUND_CHANNEL eChannel, float fSpeed)
//{
//    for (auto& pSound : m_Sounds[(_uint)eChannel])
//    {
//        pSound.second->SetSpeed(fSpeed);
//    }
//    for (auto& pSound : m_3D_Sounds[(_uint)eChannel])
//    {
//        pSound.second->SetSpeed(fSpeed);
//    }
//}
//
//void CSoundManager::Set_Sound_Speed(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fSpeed)
//{
//    shared_ptr<CSound> pSound = Find_Sound(eChannel, strSoundTag);
//    if (pSound)
//    {
//        pSound->SetSpeed(fSpeed);
//    }
//}
//
//void CSoundManager::Set_BGM_Position(_float fPosition)
//{
//    if (m_pBGM)
//    {
//        m_pBGM->SetPosition(fPosition * 100.f);
//    }
//}
//
//shared_ptr<CSound> CSoundManager::Find_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//    auto iter = m_Sounds[(_uint)eChannel].find(strSoundTag);
//    if (iter == m_Sounds[(_uint)eChannel].end())
//        return nullptr;
//    return iter->second;
//}
//
//shared_ptr<C3D_Sound> CSoundManager::Find_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag)
//{
//    auto iter = m_3D_Sounds[(_uint)eChannel].find(strSoundTag);
//    if (iter == m_3D_Sounds[(_uint)eChannel].end())
//        return nullptr;
//    return iter->second;
//}
//
//void CSoundManager::Set_Listener(shared_ptr<CGameObject> pListener)
//{
//    m_pListener = pListener;
//}
//
//shared_ptr<CSoundManager> CSoundManager::Create()
//{
//    struct MakeSharedEnabler : public CSoundManager
//    {
//        MakeSharedEnabler() : CSoundManager()
//        {}
//    };
//
//    shared_ptr<CSoundManager> pInstance = make_shared<MakeSharedEnabler>();
//    if (FAILED(pInstance->Initialize()))
//        assert(false);
//    return pInstance;
//}
//
//void CSoundManager::Free()
//{
//    for (_uint i = 0; i < (_uint)SOUND_CHANNEL::CHANNEL_END; ++i)
//    {
//        m_Sounds[i].clear();
//    }
//    for (_uint i = 0; i < (_uint)SOUND_CHANNEL::CHANNEL_END; ++i)
//    {
//        m_3D_Sounds[i].clear();
//    }
//    m_pFmodSystem->close();
//    m_pFmodSystem->release();
//}
