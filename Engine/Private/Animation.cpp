#include "..\Public\Animation.h"

#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration{ rhs.m_fDuration }
	, m_fTickPerSecond{ rhs.m_fTickPerSecond }
	, m_fCurrentTrackPosition{ rhs.m_fCurrentTrackPosition }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_Channels{ rhs.m_Channels }
	, m_isFinished{ rhs.m_isFinished }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
	, m_isRootAnim{ rhs.m_isRootAnim }
	, m_isForcedNonRootAnim{ rhs.m_isForcedNonRootAnim }
	, m_FrameCallbackList{ rhs.m_FrameCallbackList }
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto fFrame : m_FrameCallbackList)
		m_CallbackList.emplace_back([]() {});
	m_CallbackList.emplace_back([]() {});

	m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
	m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
}

CAnimation::~CAnimation()
{
	Free();
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<shared_ptr<CBone>>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;

	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		shared_ptr<CChannel> pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(shared_ptr<ANIMATION_DATA> pAIAnimation, const vector<shared_ptr<class CBone>>& Bones)
{
	strcpy(m_szName, pAIAnimation->szName.c_str());

	m_fDuration = (_float)pAIAnimation->fDuration;

	m_fTickPerSecond = (_float)pAIAnimation->fTickPerSecond;

	m_iNumChannels = pAIAnimation->iNumChannels;

	for (_float fFrame : pAIAnimation->Callback_Frames)
		m_FrameCallbackList.emplace_back(fFrame);

	m_isRootAnim = pAIAnimation->isRootAnim;
	m_isForcedNonRootAnim = pAIAnimation->isForcedNonRootAnim;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		shared_ptr<CChannel> pChannel = CChannel::Create(pAIAnimation->Channel_Datas[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<CBone>>& Bones, _bool isLoop)
{
	_float fTickTime = m_fTickPerSecond * fTimeDelta;
	for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
	{
		_float fFrame = m_FrameCallbackList[i];
		if (m_fCurrentTrackPosition < fFrame && m_fCurrentTrackPosition + m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier > fFrame)
			m_CallbackList[i]();
	}

	/* 애니메이션의 재생 위치를 증가시켜준다. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_CallbackList.back()();	// Last Callback Active

		m_isFinished = true;

		if (true == isLoop)
		{
			m_isFinished = false;
			m_fCurrentTrackPosition = 0.f;

			m_isLooped = true;
			m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
			m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
		}

		if (true == m_isFinished)
			return;
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
	}

}

void CAnimation::Update_TransformationMatrix_Blend(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop, _float fWeight)
{
	_float fTickTime = m_fTickPerSecond * fTimeDelta;
	for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
	{
		_float fFrame = m_FrameCallbackList[i];
		if (m_fCurrentTrackPosition < fFrame && m_fCurrentTrackPosition + m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier > fFrame)
			m_CallbackList[i]();
	}

	/* 애니메이션의 재생 위치를 증가시켜준다. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_CallbackList.back()();	// Last Callback Active

		m_isFinished = true;

		if (true == isLoop)
		{
			m_isFinished = false;
			m_fCurrentTrackPosition = 0.f;

			m_isLooped = true;
			m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
			m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
		}

		if (true == m_isFinished)
			return;
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update_Blend(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i], fWeight);
	}
}

void CAnimation::Update_Transformation_Aperture(_float fTimeDelta, shared_ptr<CAnimation> pTargetAnim, _float fFramePos, const vector<shared_ptr<class CBone>>& Bones)
{
	// Active setting callbacks
	for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
	{
		_float fFrame = m_FrameCallbackList[i];
		if (m_fCurrentTrackPosition < fFrame && m_fCurrentTrackPosition + m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier > fFrame)
			m_CallbackList[i]();
	}

	/* 애니메이션의 재생 위치를 증가시켜준다. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if (2 == m_Channels[i]->Get_BoneIndex())
		{
			m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
		}
		else
		{
			auto TargetFrame = pTargetAnim->m_Channels[i]->Get_Frame(fFramePos);
			m_Channels[i]->Update_Linear_Aperture(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i], TargetFrame, m_fAperture);
		}
	}
}

void CAnimation::Update_Transform_Frame(_float fFrame, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop)
{
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update(0.f, Bones, &m_CurrentKeyFrameIndices[i]);
	}

	m_fCurrentTrackPosition = fFrame;
	if (m_fCurrentTrackPosition >= m_fDuration)
		return;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
	}
}

void CAnimation::Reset_Channels(const vector<shared_ptr<class CBone>>& Bones)
{
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Reset(Bones);
	}
}

void CAnimation::Reset()
{
	m_isFinished = false;
	m_fCurrentTrackPosition = 0;
	m_CallbackList.back()();	// Last Callback Active

	for (size_t i = 0; i < m_iNumChannels; i++)
		m_CurrentKeyFrameIndices[i] = 0;

	m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
	m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
}

HRESULT CAnimation::ExportAnimInfo(const string& strFolderPath)
{
	string strFilePath = strFolderPath;
	strFilePath.append(m_szName);
	strFilePath.append(".dat");

	ofstream binFile(strFilePath, ios::out | ios::binary);
	if (!binFile.is_open())
	{
		binFile.close();
		return E_FAIL;
	}

	// Rootanimation
	write_typed_data(binFile, m_isRootAnim);

	// Callback List
	_uint iCallbackSize = m_FrameCallbackList.size();
	write_typed_data(binFile, iCallbackSize);
	for (_float fFrameCallback : m_FrameCallbackList)
		write_typed_data(binFile, fFrameCallback);
	write_typed_data(binFile, m_isForcedNonRootAnim);
	
	binFile.close();

	return S_OK;
}

shared_ptr<CAnimation> CAnimation::Create(const aiAnimation* pAIAnimation, const vector<shared_ptr<class CBone>>& Bones)
{
	MAKE_SHARED_ENABLER(CAnimation);
	shared_ptr<CAnimation> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CAnimation> CAnimation::Create(shared_ptr<ANIMATION_DATA> pAnimation, const vector<shared_ptr<class CBone>>& Bones)
{
	MAKE_SHARED_ENABLER(CAnimation);
	shared_ptr<CAnimation> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CAnimation> CAnimation::Clone()
{
	struct MakeSharedEnabler : public CAnimation
	{
		MakeSharedEnabler(const CAnimation& rhs) : CAnimation(rhs) { }
	};

	shared_ptr<CAnimation> pInstance = make_shared<MakeSharedEnabler>(*this);

	return pInstance;
}

void CAnimation::Free()
{

}
