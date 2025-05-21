//#include "3D_Sound.h"
//
//
//C3D_Sound::C3D_Sound() :
//    m_pSound(nullptr),
//    m_iVolume(0),
//    m_FmodR(FMOD_OK),
//    m_pChannel(nullptr)
//{
//}
//
//C3D_Sound::~C3D_Sound()
//{
//    Free();
//}
//
//HRESULT C3D_Sound::Initialize(FMOD::System* pSystem, const wstring& strPath)
//{
//    m_pSystem = pSystem;
//    if (!m_pSystem)
//        return E_FAIL;
//    if (FAILED(Load(strPath)))
//        return E_FAIL;
//    return S_OK;
//}
//
//HRESULT C3D_Sound::Load(const wstring& _strPath)
//{
//    if (nullptr == m_pSystem)
//    {
//        MSG_BOX(L"Fmod Failed: No System COM Object");
//        return E_FAIL;
//    }
//
//    string strTemp = string(_strPath.begin(), _strPath.end());
//    m_FmodR = m_pSystem->createSound(strTemp.c_str(), FMOD_3D, NULL, &m_pSound);
//    if (FMOD_OK != m_FmodR)
//    {
//        m_pSound->release();
//        m_pSound = nullptr;
//        wstring errMsg;
//        errMsg += L"Sound Load Failed, Error Code : ";
//        errMsg += to_wstring((int)m_FmodR);
//        errMsg += L"\nFile Path : ";
//        errMsg += _strPath;
//        MSG_BOX(errMsg.c_str());
//
//        return E_FAIL;
//    }
//
//
//    return S_OK;
//}
//
//
//void C3D_Sound::Play(_fvector vPos, _fvector vVelocity, bool _bLoop/*=false*/)
//{
//    if (nullptr == m_pSound) return;
//    m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
//    _float3 vPosF3;
//    XMStoreFloat3(&vPosF3, vPos);
//    _float3 vVel;
//    XMStoreFloat3(&vVel, vVelocity);
//    FMOD_VECTOR fvPos{ vPosF3.x, vPosF3.y,vPosF3.z };
//    FMOD_VECTOR fvVel{ vVel.x, vVel.y,vVel.z };
//    if (_bLoop)
//    {
//        m_pChannel->setMode(FMOD_LOOP_NORMAL);
//        m_pChannel->setVolume(m_fRealVolume);
//    }
//    else
//    {
//        m_pChannel->setMode(FMOD_LOOP_OFF);
//        m_pChannel->setVolume(m_fRealVolume);
//    }
//    m_pChannel->set3DAttributes(&fvPos, &fvVel);
//}
//
//void C3D_Sound::Stop()
//{
//    if (nullptr == m_pSound) return;
//    if (nullptr != m_pChannel)
//        m_pChannel->stop();
//
//}
//
//void C3D_Sound::SetPaused(bool _bPause/*=true*/)
//{
//    if (nullptr == m_pSound) return;
//    if (nullptr != m_pChannel)
//        m_pChannel->setPaused(_bPause);
//}
//
//
//void C3D_Sound::SetVolume(float _fVolume/*percentage*/)
//{
//
//    m_iVolume = GetDecibel(_fVolume);
//    if (nullptr == m_pSound) return;
//    if (nullptr != m_pChannel)
//    {
//        m_pChannel->setPaused(true);
//        m_pChannel->setVolume(_fVolume / 100.f);
//        m_pChannel->setPaused(false);
//    }
//}
//
//void C3D_Sound::SetRealVolume(float fVolume)
//{
//    m_fRealVolume = fVolume;
//}
//
//void C3D_Sound::SetPosition(float _fPosition/*percentage*/)
//{
//    if (nullptr == m_pSound) return;
//    if (nullptr != m_pChannel)
//    {
//        UINT iLen;
//        m_pSound->getLength(&iLen, FMOD_TIMEUNIT_MS);
//        _fPosition = (float)iLen * _fPosition / 100.f;
//
//        m_pChannel->setPosition((UINT)_fPosition, FMOD_TIMEUNIT_MS);
//    }
//}
//
//void C3D_Sound::SetSpeed(_float fSpeed)
//{
//    if (m_pChannel)
//    {
//        _float freq;
//        m_pChannel->getFrequency(&freq);
//        m_pChannel->setFrequency(freq * (fSpeed / m_fSpeed));
//        m_fSpeed = fSpeed;
//    }
//    else
//    {
//        m_fSpeed = 1.f;
//    }
//}
//
//bool C3D_Sound::IsPlaying()
//{
//    if (m_pChannel)
//    {
//        bool ret = false;
//        m_pChannel->isPlaying(&ret);
//        return ret;
//    }
//    return false;
//}
//
//int C3D_Sound::GetDecibel(float _fVolume)
//{
//    if (_fVolume > 100.f)
//        _fVolume = 100.f;
//    else if (_fVolume <= 0.f)
//        _fVolume = 0.00001f;
//
//    // 1 ~ 100 사이값을 데시벨 단위로 변경
//    int iVolume = (LONG)(-2000.0 * log10(100.f / _fVolume));
//
//    if (iVolume < -10000)
//        iVolume = -10000;
//    return  iVolume;
//}
//
//shared_ptr<C3D_Sound> C3D_Sound::Create(FMOD::System* pSystem, const wstring& strPath)
//{
//    struct MakeSharedEnabler : public C3D_Sound
//    {
//        MakeSharedEnabler(): C3D_Sound()
//        {
//        }
//    };
//
//    shared_ptr<C3D_Sound>  pInstance = make_shared<MakeSharedEnabler>();
//    if (FAILED(pInstance->Initialize(pSystem, strPath)))
//        assert(false);
//    return pInstance;
//}
//
//void C3D_Sound::Free()
//{
//}
