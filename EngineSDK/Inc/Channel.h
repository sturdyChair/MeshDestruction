#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

/* 1. 뼈의 정보를 가져요. */
/* 2. 뼈의 애니메이션의 재생 시간에 따른 상태 정보들을 가진다. */
class CChannel
{
private:
	CChannel();

public:
	~CChannel();

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<shared_ptr<class CBone>>& Bones);
	HRESULT Initialize(shared_ptr<CHANNEL_DATA> pAIChannel, const vector<shared_ptr<class CBone>>& Bones);
	void Update(_float fCurrentTrackPosition, const vector<shared_ptr<class CBone>>& Bones, _uint* pCurrentKeyFrameIndex);
	void Update_Linear_Aperture(_float fCurrentTrackPosition, const vector<shared_ptr<class CBone>>& Bones, _uint* pCurrentKeyFrameIndex, KEYFRAME TargetFrame, _float fAperture);

	void Reset(const vector<shared_ptr<class CBone>>& Bones) const;
	void Update_Blend(_float fCurrentTrackPosition, const vector<shared_ptr<class CBone>>& Bones, _uint* pCurrentKeyFrameIndex, _float fWeight);
	_uint Get_BoneIndex() { return  m_iBoneIndex; }
	
	KEYFRAME Get_Frame(_float fFramePos) const;
	KEYFRAME Get_Frame_Front() const { return m_KeyFrames.front(); }


private:
	_char					m_szName[MAX_PATH] = {};

	_uint					m_iNumKeyFrames = {};
	/* KEYFRAME : 이 뼈(Channel)의 시간에 따른 상태(크, 자, 이)를 표현한 구조체.*/
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iBoneIndex = { 0 };

	/* 현재 보간하고 있는 두 키프레임중 왼쪽 키프레임 */
	// _uint					m_iCurrentKeyFrameIndex = { 0 };


public:
	static shared_ptr<CChannel> Create(const aiNodeAnim* pAIChannel, const vector<shared_ptr<class CBone>>& Bones);
	static shared_ptr<CChannel> Create(shared_ptr<CHANNEL_DATA> pAIChannel, const vector<shared_ptr<class CBone>>& Bones);
	void Free();
};

END