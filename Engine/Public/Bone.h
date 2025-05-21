#pragma once

#include "Engine_Defines.h"

/* assimp에서 뼈를 표현하는 타입 : 3가지 .*/
/* aiNode, aiBone, aiNodeAnim */

/* aiNode : 뼈의 상속관계를 표현할 뿐만 아니라 뼈의 상태행렬을 표현한다. */

BEGIN(Engine)

class CBone
{
private:
	CBone();

public:
	~CBone();

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationFloat4x4Ptr() {
		return &m_CombinedTransformationMatrix;
	}
	void Set_CombinedTransformationFloat4x4Ptr(const _float4x4& CombinedTransformationMatrix) {
		m_CombinedTransformationMatrix = CombinedTransformationMatrix;
		return;
	}
	int Get_Parent() const { return m_iParentBoneIndex; }

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_fmatrix Get_TransformationMatrix() {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}
	_float4x4* Get_TransformationMatrixPtr() {
		return (&m_TransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	HRESULT Initialize(shared_ptr<NODE_DATA> BoneData, _int iParentIndex);
	void Update_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones, _fmatrix PreTransformationMatrix);
	void Update_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones, _fmatrix PreTransformationMatrix, ROOT_ANIM_PRE& RootAnimPreInfo, _float4x4& outMatrix, _bool isLooped = false);

private:
	_char				m_szName[MAX_PATH] = {};
	_float4x4			m_TransformationMatrix; /* 내 뼈만의 상태를 부모 기준으로 표현한 행렬. */
	_float4x4			m_CombinedTransformationMatrix; /* 내 뼈행렬 * 부모 뼈행렬 */
	_int				m_iParentBoneIndex = { -1 };

private:	// Only for loopAnim
	_float4x4			m_matPreFrame;

public:
	static shared_ptr<CBone> Create(const aiNode* pAINode, _int iParentIndex);
	static shared_ptr<CBone> Create(shared_ptr<NODE_DATA> BoneData, _int iParentIndex);
	shared_ptr<CBone> Clone();
	void Free();
};

END