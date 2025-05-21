#pragma once

#include "Engine_Defines.h"

/* 애니메이션의 하나(공격모션)의 정보를 가진다. */
BEGIN(Engine)

class CAnimation
{
	friend class CModel;

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);

public:
	~CAnimation();

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

	void Set_Finished(_bool isFinished) {
		m_isFinished = isFinished;
	}

	string Get_AnimName() const {
		return m_szName;
	};


public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<shared_ptr<class CBone>>& Bones);
	HRESULT Initialize(shared_ptr<ANIMATION_DATA> pAIAnimation, const vector<shared_ptr<class CBone>>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop);
	void Update_TransformationMatrix_Blend(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop, _float fWeight);
	void Update_Transformation_Aperture(_float fTimeDelta, shared_ptr<CAnimation> pTargetAnim, _float fFramePos, const vector<shared_ptr<class CBone>>& Bones);
	void Update_Transform_Frame(_float fFrame, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop);

	void Reset_Channels(const vector<shared_ptr<class CBone>>& Bones);

	void Reset();

	_float Get_Duration() const {
		return m_fDuration;
	}

	_float Get_Aperture() const {
		return m_fAperture;
	}

	void Set_Aperture(_float fAperture) {
		m_fAperture = fAperture;
	}

	_float Get_AnimationMultiplier() const {
		return m_fAnimationMultiplier;
	}

	void Set_AnimationMultiplier(_float fAnimationMultiplier) {
		m_fAnimationMultiplier = fAnimationMultiplier;
	}

	shared_ptr<class CChannel> Get_Channel(_int iIndex) {
		return m_Channels[iIndex];
	}
	_float Get_Current_Track_Posiiton() const {
		return m_fCurrentTrackPosition;
	}
	void  Set_CurrentTrackPosition(_float fCurrentTrackPosition) { m_fCurrentTrackPosition = fCurrentTrackPosition; }
	_float Get_TickPerSecond() const {
		return m_fTickPerSecond;
	}
private:
	_char				m_szName[MAX_PATH] = {};
	_float				m_fDuration = {}; /* 애니메이션의 총 재생 거리. */
	_float				m_fTickPerSecond = {}; /* 초당 재생 거리.(재생 속도) */
	_float				m_fCurrentTrackPosition = {}; /* 현재 재생 위치 */

	_float				m_fAnimationMultiplier = 1.f;
	_float				m_fAperture = 1.f;

	/* 이 애니메이션을 재생하는데 필요한 뼈의 갯수. == */
	/* 이 애님에ㅣ션이 사용하는 뼈의 갯수. */
	_uint						m_iNumChannels = {};
	vector<shared_ptr<class CChannel>>		m_Channels;
	/* 원래 채널(뼈)이 현재 어디 키프레임까지 재생된건지를 저장하고 있던 데이터를 */
	/* 애니메이션쪽(바깥)으로 빼준상황. */
	vector<_uint>				m_CurrentKeyFrameIndices;

	_bool						m_isLooped = { false };
	_bool						m_isFinished = { false };

public:	// For root animation
	ROOT_ANIM_PRE				m_RootAnimPreInfo;

public:
	HRESULT ExportAnimInfo(const string& strFilePath);

public:	// For Callback
	vector<_float>				m_FrameCallbackList;
	_bool						m_isRootAnim = false;
	_bool						m_isForcedNonRootAnim = false;
private:
	vector<function<void()>>	m_CallbackList;

public:
	static shared_ptr<CAnimation> Create(const aiAnimation* pAIAnimation, const vector<shared_ptr<class CBone>>& Bones);
	static shared_ptr<CAnimation> Create(shared_ptr<ANIMATION_DATA> pAnimation, const vector<shared_ptr<class CBone>>& Bones);
	shared_ptr<CAnimation> Clone();
	void Free();
};

END