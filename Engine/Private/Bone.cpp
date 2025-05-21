#include "..\Public\Bone.h"

CBone::CBone()
{
}

CBone::~CBone()
{
	Free();
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{

	strcpy_s(m_szName, pAINode->mName.data);

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentIndex;

	return S_OK;
}

HRESULT CBone::Initialize(shared_ptr<NODE_DATA> BoneData, _int iParentIndex)
{
	strcpy(m_szName, BoneData->szName.c_str());

	memcpy(&m_TransformationMatrix, &BoneData->TransformationMatrix, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentIndex;

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones, _fmatrix PreTransformationMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformationMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));

}

void CBone::Update_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones, _fmatrix PreTransformationMatrix, ROOT_ANIM_PRE& RootAnimPreInfo, _float4x4& outMatrix, _bool isLooped)
{
	if (-1 == m_iParentBoneIndex)	// Root Bone
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformationMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix)
		);

	_matrix RootMat = XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	_vector vCurTrans = RootMat.r[3];
	_vector vCurLook = XMVector3Normalize(RootMat.r[2]);

	//_float fScaleX = XMVectorGetX(XMVector3Length(RootMat.r[0]));
	//_float fScaleY = XMVectorGetX(XMVector3Length(RootMat.r[1]));
	//_float fScaleZ = XMVectorGetX(XMVector3Length(RootMat.r[2]));

	//RootMat = XMMatrixIdentity();
	//RootMat.r[0] *= fScaleX;
	//RootMat.r[1] *= fScaleY;
	//RootMat.r[2] *= fScaleZ;
	//RootMat *= XMMatrixRotationY(XMConvertToRadians(180.0f));

	RootMat.r[3] = { 0.f, 0.f, 0.f, 1.f };
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, RootMat);


	/*_matrix rotationMat = XMMatrixIdentity();

	_vector vPreRot = XMLoadFloat4(&RootAnimPreInfo.vPreRot);
	if (XMVectorGetX(XMVector3LengthSq(vPreRot)) > FLT_MIN &&
		XMVectorGetX(XMVector3LengthSq(vPreRot - vCurLook)) > FLT_MIN)
	{
		XMVECTOR axis = XMVector3Cross(vCurLook, vPreRot);
		axis = XMVector3Normalize(axis);

		float fAngle = acosf(XMVectorGetX(XMVector3Dot(vCurLook, vPreRot)));
		rotationMat = XMMatrixRotationAxis(axis, -fAngle);
	}*/

	_vector vPreTrans = XMLoadFloat4(&RootAnimPreInfo.vPreTrans);
	_matrix transMat = XMMatrixIdentity();
	if (XMVectorGetX(XMVector3LengthSq(vPreTrans)) > FLT_MIN)
	{
		_vector resultTrans = vCurTrans - vPreTrans;
		resultTrans = XMVectorSetW(resultTrans, 1.f);
		transMat = XMMatrixTranslationFromVector(resultTrans);
	}

	XMStoreFloat4x4(
		&outMatrix, /*rotationMat **/ transMat
	);
	
	XMStoreFloat4(&RootAnimPreInfo.vPreTrans, vCurTrans);
	XMStoreFloat4(&RootAnimPreInfo.vPreRot, vCurLook);
}

shared_ptr<CBone> CBone::Create(const aiNode* pAINode, _int iParentIndex)
{
	struct MakeSharedEnabler : public CBone
	{
		MakeSharedEnabler() : CBone() { }
	};

	shared_ptr<CBone> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CBone> CBone::Create(shared_ptr<NODE_DATA> BoneData, _int iParentIndex)
{
	struct MakeSharedEnabler : public CBone
	{
		MakeSharedEnabler() : CBone() { }
	};

	shared_ptr<CBone> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(BoneData, iParentIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CBone> CBone::Clone()
{
	return make_shared<CBone>(*this);
}

void CBone::Free()
{

}

