#pragma once

#include "Engine_Defines.h"

/* assimp���� ���� ǥ���ϴ� Ÿ�� : 3���� .*/
/* aiNode, aiBone, aiNodeAnim */

/* aiNode : ���� ��Ӱ��踦 ǥ���� �Ӹ� �ƴ϶� ���� ��������� ǥ���Ѵ�. */

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
	_float4x4			m_TransformationMatrix; /* �� ������ ���¸� �θ� �������� ǥ���� ���. */
	_float4x4			m_CombinedTransformationMatrix; /* �� ����� * �θ� ����� */
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