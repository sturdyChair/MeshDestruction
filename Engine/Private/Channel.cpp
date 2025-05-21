#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

CChannel::~CChannel()
{
	Free();
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<shared_ptr<CBone>>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(m_szName, pBone->Get_Name()))
				return true;

			++m_iBoneIndex;

			return false;
		});

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale{};
	_float4			vRotation{};
	_float3			vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = (_float)pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize(shared_ptr<CHANNEL_DATA> pAIChannel, const vector<shared_ptr<class CBone>>& Bones)
{
	strcpy_s(m_szName, pAIChannel->szNodeName.c_str());

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(m_szName, pBone->Get_Name()))
				return true;

			++m_iBoneIndex;

			return false;
		});

	m_iNumKeyFrames = max(pAIChannel->iNumScalingKeys, pAIChannel->iNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->iNumPositionKeys);

	_float3			vScale{};
	_float4			vRotation{};
	_float3			vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->iNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->tKeyFrames[i].vScale, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->tKeyFrames[i].fTime;
		}

		if (i < pAIChannel->iNumRotationKeys)
		{
			vRotation.x = pAIChannel->tKeyFrames[i].vRotation.x;
			vRotation.y = pAIChannel->tKeyFrames[i].vRotation.y;
			vRotation.z = pAIChannel->tKeyFrames[i].vRotation.z;
			vRotation.w = pAIChannel->tKeyFrames[i].vRotation.w;
			KeyFrame.fTrackPosition = (_float)pAIChannel->tKeyFrames[i].fTime;
		}

		if (i < pAIChannel->iNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->tKeyFrames[i].vTranslation, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->tKeyFrames[i].fTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update(_float fCurrentTrackPosition, const vector<shared_ptr<CBone>>& Bones, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fCurrentTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	_matrix			TransformationMatrix = XMMatrixIdentity();

	_vector			vScale, vRotation, vTranslation;
	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	if (fCurrentTrackPosition > LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio), 1.f);
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Update_Linear_Aperture(_float fCurrentTrackPosition, const vector<shared_ptr<class CBone>>& Bones, _uint* pCurrentKeyFrameIndex, KEYFRAME TargetFrame, _float fAperture)
{
	_matrix			TransformationMatrix = XMMatrixIdentity();

	_vector			vScale, vRotation, vTranslation;
	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	if (fCurrentTrackPosition > LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio), 1.f);
	}

	// 목표 KeyFrame으로 보간
	vScale = XMVectorLerp(vScale, XMLoadFloat3(&TargetFrame.vScale), fAperture);
	vRotation = XMQuaternionSlerp(vRotation, XMLoadFloat4(&TargetFrame.vRotation), fAperture);
	vTranslation = XMVectorSetW(XMVectorLerp(vTranslation, XMLoadFloat3(&TargetFrame.vTranslation), fAperture), 1.f);

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Reset(const vector<shared_ptr<class CBone>>& Bones)const
{
	Bones[m_iBoneIndex]->Set_TransformationMatrix(_matrix{ XMVectorZero(),XMVectorZero(),XMVectorZero(),XMVectorZero()});
}

void CChannel::Update_Blend(_float fCurrentTrackPosition, const vector<shared_ptr<class CBone>>& Bones, _uint* pCurrentKeyFrameIndex, _float fWeight)
{
	if (0.0f == fCurrentTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	_matrix			TransformationMatrix = XMMatrixIdentity();

	_vector			vScale, vRotation, vTranslation;
	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	if (fCurrentTrackPosition > LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio), 1.f);
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation) * fWeight;

	Bones[m_iBoneIndex]->Set_TransformationMatrix(Bones[m_iBoneIndex]->Get_TransformationMatrix() + TransformationMatrix);
}



KEYFRAME CChannel::Get_Frame(_float fFramePos) const
{
	_vector	vScale, vRotation, vTranslation;
	if (fFramePos > m_KeyFrames.back().fTrackPosition)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames.back().vTranslation), 1.f);
	}
	else
	{
		_uint iCurrentKeyFrameIndex = 0;
		while (fFramePos >= m_KeyFrames[iCurrentKeyFrameIndex + 1].fTrackPosition)
			++iCurrentKeyFrameIndex;

		_float fRatio = (fFramePos - m_KeyFrames[iCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[iCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[iCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[iCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[iCurrentKeyFrameIndex + 1].vRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex].vTranslation), XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex + 1].vTranslation), fRatio), 1.f);
	}

	KEYFRAME keyFrame{};
	XMStoreFloat3(&keyFrame.vTranslation, vTranslation);
	XMStoreFloat3(&keyFrame.vScale, vScale);
	XMStoreFloat4(&keyFrame.vRotation, vRotation);
	return keyFrame;
}

shared_ptr<CChannel> CChannel::Create(const aiNodeAnim* pAIChannel, const vector<shared_ptr<class CBone>>& Bones)
{
	MAKE_SHARED_ENABLER(CChannel);
	shared_ptr<CChannel> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CChannel> CChannel::Create(shared_ptr<CHANNEL_DATA> pAIChannel, const vector<shared_ptr<class CBone>>& Bones)
{
	MAKE_SHARED_ENABLER(CChannel);
	shared_ptr<CChannel> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		assert(false);
	}

	return pInstance;
}


void CChannel::Free()
{

}

