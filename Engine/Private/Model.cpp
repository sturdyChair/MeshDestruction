#include "Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include <filesystem>
#include "Channel.h"
#include "hwiVoro/Container.h"
#include "hwiVoro/c_loop.h"
#include "Math_Manager.h"
#include "CSG_Manager.h"
#include "iostream"
#include "Timer.h"
#include "BlastManager.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	/*, m_Bones{ rhs.m_Bones }*/
	, m_iNumAnimations{ rhs.m_iNumAnimations }
	/*, m_Animations {rhs.m_Animations }*/
	, m_TransformMatrix{ rhs.m_TransformMatrix }
	, m_eModelType{ rhs.m_eModelType }
	, m_AnimMapping{ rhs.m_AnimMapping }
	, m_pModelData{ rhs.m_pModelData }
{
	_uint iAnimIndex = 0;
	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		m_Animations.emplace_back(pPrototypeAnimation->Clone());
		m_AnimMapping.emplace(m_Animations[iAnimIndex]->Get_AnimName(), iAnimIndex);
		++iAnimIndex;
	}


	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());
}

CModel::~CModel()
{
	Free();
}

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	m_pModelData = make_shared<MODEL_DATA>();
	m_pModelData->Make_ModelData(pModelFilePath, eType, TransformMatrix);
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	/* aiProcess_PreTransformVertices : 내 모델은 여러개의 메시(칼, 몸, 가방, 모닝스타, 그림자)로 구분되어있어. */
	/* 이 각각의 메시를 배치되야할 상태대로 다 미리 셋팅하여 로드한다. */
	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (eType == MODEL_TYPE::NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	Init_Model(pModelFilePath, "", 0);

	///* 모든 모델의 정보를 다 읽어서 aiScene안에 저장해둔 것이다. */
	//m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	//if (nullptr == m_pAIScene)
	//	return E_FAIL;


	//XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	///* aiScene에 있는 정보를 내가 활용하기 좋도록 수정 정리 보관하는 작업을 수행하자. */
	//if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
	//	return E_FAIL;

	///* 이 모델을 구성하는 각각의 메시를 로드하자. */
	///* 애님 모델일 경우, 현재 메시가 사용하는 뼈의 인덱스를 메시에 보관한다. */
	//if (FAILED(Ready_Meshes()))
	//	return E_FAIL;

	///* 빛 정보 <-> 정점의 노말, (Material) 픽셀당 무슨 색을 반사해야할지?! 픽셀당 환경광원을 어떻게 설정할지? 픽셀당 스펙큘러 연산을 어떻게해야할지?! */
	//if (FAILED(Ready_Materials(pModelFilePath)))
	//	return E_FAIL;

	//if (FAILED(Ready_Animations()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	/* old : 모든 메시를 순회하면서 각 메시의 렌더함수를 호출한다. */
	/* Curr : 메시의 순회를 클라이언트에서 수행을 하고 지정한 메시에 대해서만 렌더함수를 호출한다.  */
	/* 어떤 머테리얼(Diffuse Ambient)을 어떤 쉐이더에 어떤 변수에 전달할 건지를 결정을 해야하는 작업을 수행해야했다. */
	/* 어떤 머테리얼인지가 메시마다 다르다. CMesh::m_iMaterialIndex */

	//for (size_t i = 0; i < m_iNumMeshes; i++)
	//{
	//	/* 쉐이더에 텍스쳐를 던지느 ㄴ작업을 클라이언트에서 처리하기위해 위의 루프자체를 클라로 옮긴것이다. */
	//	m_Materials[m_Meshes[i]->Get_MaterialIndex()].pTextures[aiTextureType_DIFFUSE]->Bind_ShaderResource();

	//	m_Meshes[i]->Render();
	//}

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Chander_Material(_uint idx, shared_ptr<CTexture> pTexture, aiTextureType eType)
{
	m_Materials[idx].pTextures[eType] = pTexture;
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationFloat4x4Ptr();
}

void CModel::Set_BoneMatrixPtr(const _char* pBoneName, const _float4x4& Transform)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return;
	(*iter)->Set_CombinedTransformationFloat4x4Ptr(Transform);
	return;
}

void CModel::Set_BoneMatrix(_uint iBoneIdx, const _float4x4& Transform)
{
	Set_BoneMatrix(iBoneIdx, XMLoadFloat4x4(&Transform));
}

void CModel::Set_BoneMatrix(_uint iBoneIdx, _fmatrix Transform)
{
	m_Bones[iBoneIdx]->Set_TransformationMatrix(Transform);
}

const _float4x4* CModel::Get_BoneBasicMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_TransformationMatrixPtr();
}

const _vector CModel::Get_BonePosition(const _char* pBoneName, _vector Pos) const
{
	/*auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return;*/
	return _vector();
}

string CModel::Get_BoneName(_uint idx)
{
	return m_Bones[idx]->Get_Name();
}

void CModel::Play_Animation(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix)
{
	if (m_fRemainTime > 0.f)
	{
		m_fRemainTime = m_fRemainTime - fTimeDelta * (1.f / fInterPoleTime);
		if (m_fRemainTime > 0.f)
		{
			m_Animations[m_iCurrentAnimIndex]->Set_Aperture(m_fRemainTime);
			m_Animations[m_iCurrentAnimIndex]->Update_Transformation_Aperture(fPlayTimeDelta, m_Animations[m_iPreAnimIndex], m_fPreFramePos, m_Bones);
		}
		else
		{
			m_iPreAnimIndex = -1;
			m_Animations[m_iCurrentAnimIndex]->Set_Aperture(0.f);
			m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fPlayTimeDelta, m_Bones, m_isLoop);
		}
	}
	else
	{
		m_iPreAnimIndex = -1.f;
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fPlayTimeDelta, m_Bones, m_isLoop);
	}

	// For Root Animation
	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = 2;// m_Animations[m_iCurrentAnimIndex]->Get_Channel(0)->Get_BoneIndex();
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Play_Non_InterPole_Animation(_float fTimeDelta, _float4x4& outMatrix)
{
	m_iPreAnimIndex = -1.f;
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);

	// For Root Animation
	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = 2;// m_Animations[m_iCurrentAnimIndex]->Get_Channel(0)->Get_BoneIndex();
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Set_AnimFrame(_float fFramePos)
{
	m_iPreAnimIndex = -1.f;
	m_Animations[m_iCurrentAnimIndex]->Update_Transform_Frame(fFramePos, m_Bones, m_isLoop);

	// For Root Animation
	_float4x4 outMatrix;

	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = 2;// m_Animations[m_iCurrentAnimIndex]->Get_Channel(0)->Get_BoneIndex();
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Init_Model(const char* sModelKey, const string& szTexturePath, _uint iTimeScaleLayer)
{
	//Reset_Model();

	//m_pModelData = GAMEINSTANCE->Get_ModelFromKey(sModelKey);

	if (!m_pModelData)
		assert(false);

	m_szModelKey = sModelKey;

	Init_Model_Internal(m_pModelData, szTexturePath, iTimeScaleLayer);

}

HRESULT CModel::Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	if (m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType] == nullptr)
		return E_FAIL;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_Mat_MatIdx(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	if (m_Materials[iMeshIndex].pTextures[eMaterialType] == nullptr)
		return E_FAIL;

	return m_Materials[iMeshIndex].pTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

void CModel::Init_Model_Internal(shared_ptr<MODEL_DATA> pModelData, const string& szTexturePath, _uint iTimeScaleLayer)
{
	std::vector<std::thread> threads;

	m_iNumMeshes = pModelData->iNumMeshs;
	m_eModelType = pModelData->eModelType;
	//XMStoreFloat4x4(&m_TransformMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	//XMStoreFloat4x4(&m_TransformMatrix, XMLoadFloat4x4(&pModelData->TransformMatrix));
	//XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

	if (szTexturePath.empty())
	{
		char szDir[MAX_PATH];
		_splitpath_s(m_pModelData->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		threads.emplace_back(&CModel::Create_Materials, this, szDir);
		//Create_Materials(szDir);
	}
	else
	{
		threads.emplace_back(&CModel::Create_Materials, this, szTexturePath.c_str());
		//Create_Materials(szTexturePath.c_str());
	}

	//threads.emplace_back(&CModel::Create_Bone, this, m_pModelData.get()->RootNode, 0);
	Create_Bone(m_pModelData.get()->RootNode, -1);

	/*for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pModelData->Mesh_Datas[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return;

		m_Meshes.emplace_back(pMesh);
	}*/
	threads.emplace_back(&CModel::Mesh_Array, this, pModelData);

	//std::sort(m_pBoneNodes.begin(), m_pBoneNodes.end(), [](weak_ptr<CBoneNode> pSour, weak_ptr<CBoneNode> pDest)
	//	{
	//		return pSour.lock()->Get_Depth() < pDest.lock()->Get_Depth();
	//	});

	for (auto& th : threads) {
		if (th.joinable()) {
			th.join();
		}
	}

	//threads.emplace_back(&CModel::Create_Animations, this);
	Create_Animations();



}

HRESULT CModel::Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex)
{
	/* 쉐이더에 던져야 할 행렬 배열정보. */
	_float4x4	BoneMatrices[512];

	/* 행렬배열은 메시에게 영향을 주는 뼈들의 행렬만 가져야하므로 메시에게 저장좀해줘! */
	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}

_bool CModel::Push_Back_Mesh(shared_ptr<MESH_DATA> pMeshData)
{
	auto pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pMeshData, m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	if (!pMesh)
		return false;

	m_pModelData->iNumMeshs += 1;
	m_pModelData->Mesh_Datas.push_back(pMeshData);

	m_iNumMeshes += 1;
	m_Meshes.push_back(pMesh);
	return true;
}
_bool CModel::Push_Back_Mesh(shared_ptr<CMesh> pMesh)
{
	m_iNumMeshes += 1;
	m_Meshes.push_back(pMesh);
	return true;
}
_bool CModel::Replace_Mesh(shared_ptr<MESH_DATA> pMeshData, _uint iIdx, _bool bPreTrans)
{
	if (iIdx >= m_iNumMeshes)
		return false;
	shared_ptr<CMesh> pMesh;
	if(bPreTrans)
		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pMeshData, m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	else
		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pMeshData, m_Bones, XMLoadFloat4x4(&Identity));
	if (!pMesh)
		return false;
	m_pModelData->Mesh_Datas[iIdx] = pMeshData;
	m_Meshes[iIdx] = pMesh;
	return true;
}
_bool CModel::Replace_Mesh(shared_ptr<CMesh> pMesh, _uint iIdx)
{
	if (iIdx >= m_iNumMeshes)
		return false;
	m_Meshes[iIdx] = pMesh;
	return true;
}
HRESULT CModel::ExportAnimInfo(const string& strFolderPath)
{
	return m_Animations[Get_CurrentAnimIndex()]->ExportAnimInfo(strFolderPath);
}

HRESULT CModel::SetAnimCallback(const string& strAnimName, _uint iCallbackPos, function<void()> callback)
{
	if (m_AnimMapping.find(strAnimName) == m_AnimMapping.end())
		return E_FAIL;

	_uint iAnimIndex = m_AnimMapping[strAnimName];
	if (m_Animations[iAnimIndex]->m_FrameCallbackList.size() == 0 ||
		m_Animations[iAnimIndex]->m_FrameCallbackList.size() <= iCallbackPos
		)
		return E_FAIL;

	m_Animations[iAnimIndex]->m_CallbackList[iCallbackPos] = callback;
	return S_OK;

}

HRESULT CModel::SetLastAnimCallback(const string& strAnimName, function<void()> callback)
{
	if (m_AnimMapping.find(strAnimName) == m_AnimMapping.end())
		return E_FAIL;

	_uint iAnimIndex = m_AnimMapping[strAnimName];
	if (m_Animations[iAnimIndex]->m_FrameCallbackList.size() == 0)
		return E_FAIL;

	m_Animations[iAnimIndex]->m_CallbackList.back() = callback;
	return S_OK;
}

void CModel::UpdateVertexInfo()
{
	_float3 vMax{ -FLT_MAX,-FLT_MAX,-FLT_MAX };
	_float3 vMin{ FLT_MAX,FLT_MAX,FLT_MAX };
	if(m_eModelType == MODEL_TYPE::NONANIM)
	{
		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			_uint iNum = m_pModelData->Mesh_Datas[i]->iNumVertices;
			for (_uint j = 0; j < iNum; ++j)
			{
				if (vMax.x < m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.x)
					vMax.x = m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.x;
				if (vMax.y < m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.y)
					vMax.y = m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.y;
				if (vMax.z < m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.z)
					vMax.z = m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.z;
				if (vMin.x > m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.x)
					vMin.x = m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.x;
				if (vMin.y > m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.y)
					vMin.y = m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.y;
				if (vMin.z > m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.z)
					vMin.z = m_pModelData->Mesh_Datas[i]->pVertices[j].vPosition.z;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			_uint iNum = m_pModelData->Mesh_Datas[i]->iNumVertices;
			for (_uint j = 0; j < iNum; ++j)
			{
				if (vMax.x < m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.x)
					vMax.x = m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.x;
				if (vMax.y < m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.y)
					vMax.y = m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.y;
				if (vMax.z < m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.z)
					vMax.z = m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.z;
				if (vMin.x > m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.x)
					vMin.x = m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.x;
				if (vMin.y > m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.y)
					vMin.y = m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.y;
				if (vMin.z > m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.z)
					vMin.z = m_pModelData->Mesh_Datas[i]->pAnimVertices[j].vPosition.z;
			}
		}
	}
	m_pModelData->VertexInfo.vMax = vMax;
	m_pModelData->VertexInfo.vMin = vMin;
	m_pModelData->VertexInfo.vCenter = {(vMax.x + vMin.x) * 0.5f,(vMax.y + vMin.y) * 0.5f,(vMax.z + vMin.z) * 0.5f };
}

void CModel::Reset_Model()
{
	m_pModelData.reset();
	m_szModelKey.clear();

	m_Meshes.clear();
	m_Materials.clear();
	m_Bones.clear();
	m_Animations.clear();

	m_iCurrentAnimIndex = 0;
	m_iNumAnimations = 0;
	m_iNumMaterials = 0;
}

void CModel::Reset_Bones()
{
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Reset_Bones_PreTransform(_fmatrix pret)
{
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, pret);
	}
}

void CModel::Play_Animation_Blend(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix)
{
	m_Animations[m_vecAnimIndex[0]]->Reset_Channels(m_Bones);
	for(_uint i = 0; i < m_vecAnimIndex.size(); ++i)
	{
		if (m_fRemainTime > 0.f)
		{
			m_fRemainTime = m_fRemainTime - fTimeDelta * (1.f / fInterPoleTime);
			//if (m_fRemainTime > 0.f)
			//{
			//	m_Animations[m_iCurrentAnimIndex]->Set_Aperture(m_fRemainTime);
			//	m_Animations[m_iCurrentAnimIndex]->Update_Transformation_Aperture(fPlayTimeDelta, m_Animations[m_iPreAnimIndex], m_fPreFramePos, m_Bones);
			//}
			//else
			{
				//m_iPreAnimIndex = -1;
				m_Animations[m_vecAnimIndex[i]]->Set_Aperture(0.f);
				m_Animations[m_vecAnimIndex[i]]->Update_TransformationMatrix_Blend(fPlayTimeDelta, m_Bones, m_isLoop, m_vecAnimWeight[i]);
			}
		}
		else
		{
			m_iPreAnimIndex = -1.f;
			m_Animations[m_vecAnimIndex[i]]->Update_TransformationMatrix_Blend(fPlayTimeDelta, m_Bones, m_isLoop, m_vecAnimWeight[i]);
		}
	}

	// For Root Animation
	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = 2;// m_Animations[m_iCurrentAnimIndex]->Get_Channel(0)->Get_BoneIndex();
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Set_AnimIndex_Blend(vector<_uint>& AnimIndices, vector<_float>& AnimWeights, _bool bLoop)
{
	m_vecPreAnimIndex = m_vecAnimIndex;
	m_vecAnimWeight = AnimWeights;
	m_vecPreFramePos.clear();
	for(auto iPreAnimIdx : m_vecPreAnimIndex)
		m_vecPreFramePos.push_back(m_Animations[iPreAnimIdx]->m_fCurrentTrackPosition);

	m_vecAnimIndex = AnimIndices;
	for(auto iCurrAnimIdx : m_vecAnimIndex)
		m_Animations[iCurrAnimIdx]->Reset();


	m_fRemainTime = 1.f;

	m_isLoop = bLoop;
}

void CModel::Set_AnimIndex_Blend(vector<string>& AnimIndices, vector<_float>& AnimWeights, _bool bLoop)
{
	vector<_uint> Indices;
	Indices.reserve(AnimIndices.size());
	for (const auto& tag : AnimIndices)
	{
		Indices.push_back(m_AnimMapping[tag]);
	}
	Set_AnimIndex_Blend(Indices, AnimWeights, bLoop);
}
pair<vector<_bool>,vector<_bool>> CModel::Divide(_uint iMeshIdx, const vector<VTXANIMMESH>& nexVertices, const vector<_uint3>& Up, const vector<_uint3>& Down, const _float4& plane)
{
	if (m_iNumMeshes < iMeshIdx)
		return {};
	_uint iNumVertices = m_pModelData->Mesh_Datas[iMeshIdx]->iNumVertices;
	_float4 revPlane = { -plane.x,-plane.y,-plane.z,-plane.w };
	vector<VTXANIM> upVert;		vector<_uint>   upInd;
	Reconstruct_Buffer(iMeshIdx, nexVertices, Up, upVert, upInd, revPlane);
	vector<VTXANIM> downVert;	vector<_uint>   downInd;
	Reconstruct_Buffer(iMeshIdx, nexVertices, Down, downVert, downInd, plane);

	auto pUpMD = Reconstruct_MeshData(iMeshIdx, upVert, upInd);
	auto pDownMD = Reconstruct_MeshData(iMeshIdx, downVert, downInd);
	pUpMD->szName = m_pModelData->Mesh_Datas[iMeshIdx]->szName + string("_") + to_string(m_iSplitCount);
	pDownMD->szName = m_pModelData->Mesh_Datas[iMeshIdx]->szName + string("_") + to_string(m_iSplitCount + 1);
	auto pUpMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pUpMD, m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	auto pDownMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pDownMD, m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	m_pModelData->Mesh_Datas[iMeshIdx] = pUpMD;
	m_pModelData->Mesh_Datas.push_back(pDownMD);
	m_pModelData->iNumMeshs += 1;
	
	m_iNumMeshes += 1;
	m_Meshes[iMeshIdx] = pUpMesh;
	m_Meshes.push_back(pDownMesh);

	m_iSplitCount += 2;

	return {Valid_Bones(upVert), Valid_Bones(downVert)};
}
void CModel::Divide(_uint iMeshIdx, const vector<VTXMESH>& nexVertices, const vector<_uint3>& Up, const vector<_uint3>& Down, const _float4& plane)
{
	if (m_iNumMeshes < iMeshIdx)
		return;
	_uint iNumVertices = m_pModelData->Mesh_Datas[iMeshIdx]->iNumVertices;
	_float4 revPlane = { -plane.x,-plane.y,-plane.z,-plane.w };
	vector<VTXMESH> upVert;		vector<_uint>   upInd;
	Reconstruct_Buffer(iMeshIdx, nexVertices, Up, upVert, upInd, revPlane);
	vector<VTXMESH> downVert;	vector<_uint>   downInd;
	Reconstruct_Buffer(iMeshIdx, nexVertices, Down, downVert, downInd, plane);

	auto pUpMD = Reconstruct_MeshData(iMeshIdx, upVert, upInd);
	auto pDownMD = Reconstruct_MeshData(iMeshIdx, downVert, downInd);
	pUpMD->szName = m_pModelData->Mesh_Datas[iMeshIdx]->szName + string("_") + to_string(m_iSplitCount);
	pDownMD->szName = m_pModelData->Mesh_Datas[iMeshIdx]->szName + string("_") + to_string(m_iSplitCount + 1);
	auto pUpMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pUpMD, m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	auto pDownMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pDownMD, m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	m_pModelData->Mesh_Datas[iMeshIdx] = pUpMD;
	m_pModelData->Mesh_Datas.push_back(pDownMD);
	m_pModelData->iNumMeshs += 1;

	m_iNumMeshes += 1;
	m_Meshes[iMeshIdx] = pUpMesh;
	m_Meshes.push_back(pDownMesh);

	m_iSplitCount += 2;

	return;
}
void CModel::Reconstruct_Buffer(_uint iMeshIdx, const vector<VTXANIMMESH>& nexVertices, const vector<_uint3>& Indicies, 
	vector<VTXANIM>& outVert, vector<_uint>& outInd,const _float4& normal)
{
	_uint iNumVertices = m_pModelData->Mesh_Datas[iMeshIdx]->iNumVertices;
	outVert.reserve(iNumVertices);
	{
		map<_uint, vector<_uint>> newFaceEdge;
		unordered_map<posBlendIdx, _uint, hash<posBlendIdx>, posEq> hashIndex;

		map<_uint, _uint> indexMap;
		for (auto& tri : Indicies)
		{
			array<_uint, 3> triangle = { tri.x, tri.y, tri.z };
			vector<pair<_uint, _uint>> newVertIndex;
			_uint idx = 0;
			for (auto i : triangle)
			{
				auto it = indexMap.find(i);
				if (it == indexMap.end())
				{
					_uint newIndex = outVert.size();
					if (i < iNumVertices)
					{
						outVert.push_back({ m_pModelData->Mesh_Datas[iMeshIdx]->pAnimVertices[i] });
					}
					else
					{
						outVert.push_back({ nexVertices[i - iNumVertices] });
						auto iter = hashIndex.find({ outVert.back().vPosition, outVert.back().vBlendIndex});
						if (iter == hashIndex.end())
						{
							hashIndex.insert({ { outVert.back().vPosition, outVert.back().vBlendIndex}, newIndex});
							newVertIndex.push_back({ idx, newIndex });
						}
						else
						{
							newVertIndex.push_back({ idx, iter->second });
						}
					}
					outInd.push_back(newIndex);
					indexMap[i] = newIndex;
				}
				else
				{
					if (i >= iNumVertices)
					{
						auto iter = hashIndex.find({ nexVertices[i - iNumVertices].vPosition,  nexVertices[i - iNumVertices].vBlendIndices });
						{
							newVertIndex.push_back({ idx, iter->second });
						}
					}
					outInd.push_back(it->second);
				}
				++idx;
			}
			if (newVertIndex.size() == 2)
			{
				if ((newVertIndex[0].first + 1)== newVertIndex[1].first)
				{
					newFaceEdge[newVertIndex[0].second].push_back(newVertIndex[1].second);
				}
				else
				{
					newFaceEdge[newVertIndex[1].second].push_back(newVertIndex[0].second);
				}
			}
		}
		vector<_uint> currPath;
		map<_uint, _uint> pathIdx;
		vector<vector<_uint>> cycles;
		set<_uint> visited;
		for (auto& nfeIdx : newFaceEdge)
		{
			if (visited.count(nfeIdx.first))
			{
				continue;
			}
			visited.insert(nfeIdx.first);
			Make_NewFace(newFaceEdge, currPath, pathIdx, cycles, nfeIdx.first, visited);
		}
		_float3 norm = { normal.x,  normal.y,  normal.z };
		enum UVPlanar {XY, YZ, ZX};
		UVPlanar planar{};
		if (norm.z > max(norm.x, norm.y))
			planar = XY;
		else if (norm.x > max(norm.y, norm.z))
			planar = YZ;
		else
			planar = ZX;
		const auto& vMax = m_pModelData->VertexInfo.vMax;
		const auto& vMin = m_pModelData->VertexInfo.vMin;
		for (auto& cycle : cycles)
		{
			if (cycle.size() <= 3)
				continue;
			vector<_float3> vertPosForEarClip;
			for (auto& idx : cycle)
			{
				auto vert = outVert[idx];
				vert.vNormal = norm;
				switch (planar)
				{
				case XY:
				{
					vert.vTexcoord.x = (vert.vPosition.x - vMin.x) / (vMax.x - vMin.x) + 1.f;
					vert.vTexcoord.y = (vert.vPosition.y - vMin.y) / (vMax.y - vMin.y) + 1.f;
				}
					break;
				case YZ:
				{
					vert.vTexcoord.x = (vert.vPosition.y - vMin.y) / (vMax.y - vMin.y) + 1.f;
					vert.vTexcoord.y = (vert.vPosition.z - vMin.z) / (vMax.z - vMin.z) + 1.f;
				}
					break;
				case ZX:
				{
					vert.vTexcoord.x = (vert.vPosition.z - vMin.z) / (vMax.z - vMin.z) + 1.f;
					vert.vTexcoord.y = (vert.vPosition.x - vMin.x) / (vMax.x - vMin.x) + 1.f;
				}
					break;
				default:
					vert.vTexcoord = { 1.f,1.f };
					break;
				}

				idx = outVert.size();
				outVert.push_back(vert);
				vertPosForEarClip.push_back(vert.vPosition);
			}
			auto nfTriangles = triangulate3DPolygon(vertPosForEarClip, norm);
			for (auto [_x,_y,_z] : nfTriangles)
			{
				outInd.push_back(cycle[_x]);
				outInd.push_back(cycle[_y]);
				outInd.push_back(cycle[_z]);
			}

			//for (_int i = 0; i < cycle.size() - 2; ++i)
			//{
			//	outInd.push_back(cycle[0]);
			//	outInd.push_back(cycle[i + 1]);
			//	outInd.push_back(cycle[i + 2]);
			//}
		}

		//for (auto& cycle : cycles)
		//{
		//	for(_uint i = 0; i < cycle.size() - 2; ++ i)
		//	{
		//		outInd.push_back(cycle[0]);
		//		outInd.push_back(cycle[i + 1]);
		//		outInd.push_back(cycle[i + 2]);
		//	}
		//}
	}
}

void CModel::Reconstruct_Buffer(_uint iMeshIdx, const vector<VTXMESH>& nexVertices, const vector<_uint3>& Indicies,
	vector<VTXMESH>& outVert, vector<_uint>& outInd, const _float4& normal)
{
	_uint iNumVertices = m_pModelData->Mesh_Datas[iMeshIdx]->iNumVertices;
	outVert.reserve(iNumVertices);
	{
		
		map<_uint, vector<_uint>> newFaceEdge;
		unordered_map<_float3, _uint, hash<_float3>, fltEq> hashIndex;

		map<_uint, _uint> indexMap;
		for (auto& tri : Indicies)
		{
			array<_uint, 3> triangle = { tri.x, tri.y, tri.z };
			vector<pair<_uint, _uint>> newVertIndex;
			_uint idx = 0;
			for (auto i : triangle)
			{
				auto it = indexMap.find(i);
				if (it == indexMap.end())
				{
					_uint newIndex = outVert.size();
					if (i < iNumVertices)
					{
						outVert.push_back({ m_pModelData->Mesh_Datas[iMeshIdx]->pVertices[i] });
					}
					else
					{
						outVert.push_back({ nexVertices[i - iNumVertices] });
						auto iter = hashIndex.find( outVert.back().vPosition);
						if (iter == hashIndex.end())
						{
							hashIndex.insert({ outVert.back().vPosition, newIndex });
							newVertIndex.push_back({ idx, newIndex });
						}
						else
						{
							newVertIndex.push_back({ idx, iter->second });
						}
					}
					outInd.push_back(newIndex);
					indexMap[i] = newIndex;
				}
				else
				{
					if (i >= iNumVertices)
					{
						auto iter = hashIndex.find( nexVertices[i - iNumVertices].vPosition);
						{
							newVertIndex.push_back({ idx, iter->second });
						}
					}
					outInd.push_back(it->second);
				}
				++idx;
			}
			if (newVertIndex.size() == 2)
			{
				if (newVertIndex[0].second == newVertIndex[1].second)
					continue;
				if ((newVertIndex[0].first + 1) == newVertIndex[1].first)
				{
					newFaceEdge[newVertIndex[0].second].push_back(newVertIndex[1].second);
				}
				else
				{
					newFaceEdge[newVertIndex[1].second].push_back(newVertIndex[0].second);
				}
			}
		}
		vector<_uint> currPath;
		map<_uint, _uint> pathIdx;
		vector<vector<_uint>> cycles;
		set<_uint> visited;
		for (auto& nfeIdx : newFaceEdge)
		{
			if (visited.count(nfeIdx.first))
			{
				continue;
			}
			visited.insert(nfeIdx.first);
			Make_NewFace(newFaceEdge, currPath, pathIdx, cycles, nfeIdx.first, visited);
		}
		_float3 norm = { normal.x,  normal.y,  normal.z };
		enum UVPlanar { XY, YZ, ZX };
		UVPlanar planar{};
		if (norm.z > max(norm.x, norm.y))
			planar = XY;
		else if (norm.x > max(norm.y, norm.z))
			planar = YZ;
		else
			planar = ZX;
		const auto& vMax = m_pModelData->VertexInfo.vMax;
		const auto& vMin = m_pModelData->VertexInfo.vMin;
		for (auto& cycle : cycles)
		{
			if (cycle.size() <= 3)
				continue;
			vector<_float3> vertPosForEarClip;
			for (auto& idx : cycle)
			{
				auto vert = outVert[idx];
				vert.vNormal = norm;
				switch (planar)
				{
				case XY:
				{
					vert.vTexcoord.x = (vert.vPosition.x - vMin.x) / (vMax.x - vMin.x) + 1.f;
					vert.vTexcoord.y = (vert.vPosition.y - vMin.y) / (vMax.y - vMin.y) + 1.f;
				}
				break;
				case YZ:
				{
					vert.vTexcoord.x = (vert.vPosition.y - vMin.y) / (vMax.y - vMin.y) + 1.f;
					vert.vTexcoord.y = (vert.vPosition.z - vMin.z) / (vMax.z - vMin.z) + 1.f;
				}
				break;
				case ZX:
				{
					vert.vTexcoord.x = (vert.vPosition.z - vMin.z) / (vMax.z - vMin.z) + 1.f;
					vert.vTexcoord.y = (vert.vPosition.x - vMin.x) / (vMax.x - vMin.x) + 1.f;
				}
				break;
				default:
					vert.vTexcoord = { 1.f,1.f };
					break;
				}

				idx = outVert.size();
				outVert.push_back(vert);
				vertPosForEarClip.push_back(vert.vPosition);
			}
			auto nfTriangles = triangulate3DPolygon(vertPosForEarClip, norm);
			for (auto [_x, _y, _z] : nfTriangles)
			{
				outInd.push_back(cycle[_x]);
				outInd.push_back(cycle[_y]);
				outInd.push_back(cycle[_z]);
			}

		}


	}
}

shared_ptr<MESH_DATA> CModel::Reconstruct_MeshData(_uint iOriMeshIdx, const vector<VTXANIM>& vert, const vector<_uint>& indicies)
{
	auto& oriMD = m_pModelData->Mesh_Datas[iOriMeshIdx];
	shared_ptr<MESH_DATA> rt = make_shared<MESH_DATA>();
	rt->iNumVertices = vert.size();
	rt->iNumFaces = indicies.size() / 3;
	rt->pAnimVertices = shared_ptr<VTXANIM[]>(new VTXANIM[rt->iNumVertices]);
	memcpy((rt->pAnimVertices.get()), vert.data(), sizeof(VTXANIM) * rt->iNumVertices);
	rt->pIndices = shared_ptr<FACEINDICES32[]>(new FACEINDICES32[rt->iNumFaces]);
	for (_uint i = 0; i < rt->iNumFaces; ++i)
	{
		rt->pIndices[i]._1 = indicies[i * 3];
		rt->pIndices[i]._2 = indicies[i * 3 + 1];
		rt->pIndices[i]._3 = indicies[i * 3 + 2];
		rt->pIndices[i].numIndices = 3;
	}
	
	rt->Bone_Datas = oriMD->Bone_Datas;
	rt->eModelType = MODEL_TYPE::ANIM;
	rt->iMaterialIndex = oriMD->iMaterialIndex;
	rt->iNumBones = oriMD->iNumBones;

	return rt;
}
shared_ptr<MESH_DATA> CModel::Reconstruct_MeshData(_uint iOriMeshIdx, const vector<VTXMESH>& vert, const vector<_uint>& indicies)
{
	auto& oriMD = m_pModelData->Mesh_Datas[iOriMeshIdx];
	shared_ptr<MESH_DATA> rt = make_shared<MESH_DATA>();
	rt->iNumVertices = vert.size();

	rt->pVertices = shared_ptr<VTXMESH[]>(new VTXMESH[rt->iNumVertices]);
	memcpy((rt->pVertices.get()), vert.data(), sizeof(VTXMESH) * rt->iNumVertices);
	if(indicies.size())
	{
		rt->iNumFaces = indicies.size() / 3;
		rt->pIndices = shared_ptr<FACEINDICES32[]>(new FACEINDICES32[rt->iNumFaces]);
		for (_uint i = 0; i < rt->iNumFaces; ++i)
		{
			rt->pIndices[i]._1 = indicies[i * 3];
			rt->pIndices[i]._2 = indicies[i * 3 + 1];
			rt->pIndices[i]._3 = indicies[i * 3 + 2];
			rt->pIndices[i].numIndices = 3;
		}
	}
	else
	{
		rt->iNumFaces = oriMD->iNumFaces;
		rt->pIndices = shared_ptr<FACEINDICES32[]>(new FACEINDICES32[rt->iNumFaces]);
		memcpy((rt->pIndices.get()),oriMD->pIndices.get(), sizeof(FACEINDICES32) * rt->iNumFaces);
	}

	rt->Bone_Datas = oriMD->Bone_Datas;
	rt->eModelType = MODEL_TYPE::NONANIM;
	rt->iMaterialIndex = oriMD->iMaterialIndex;
	rt->iNumBones = oriMD->iNumBones;

	return rt;
}
void CModel::Make_NewFace(const map<_uint, vector<_uint>>& newFaceEdge, vector<_uint>& currPath, map<_uint, _uint>& pathIdx, vector<vector<_uint>>& cycles, _uint currVertex, set<_uint>& visited)
{
	currPath.push_back(currVertex);
	auto iter = pathIdx.find(currVertex);
	if (iter != pathIdx.end())
	{
		cycles.push_back(vector(currPath.begin() + iter->second, currPath.end() - 1));
		cout << "true cycle : " << cycles.back().size() <<"\n";
	}
	else
	{
		auto currIter = (newFaceEdge.find(currVertex));
		if (currIter == newFaceEdge.end() || currIter->second.size() == 0)
		{
			cycles.push_back(vector(currPath.begin(), currPath.end()));
			//cycles.back().push_back(currVertex);
			cout << "dead end : " << cycles.back().size() << "\n";
		}
		else
		{
			pathIdx.insert({ currVertex, currPath.size() - 1 });
			set<_uint> dup;
			for (auto next : currIter->second)
			{
				if (next == currVertex || dup.count(next))
					continue;
				dup.insert(next);
				visited.insert(next);
				Make_NewFace(newFaceEdge, currPath, pathIdx, cycles, next, visited);
			}
			pathIdx.erase(currVertex);
		}
	}
	currPath.pop_back();
	return;
}
vector<_bool> CModel::Valid_Bones(const vector<VTXANIM>& vertices)
{
	vector<_bool> rt(m_Bones.size(), false);

	//for (auto& vertex : vertices)
	//{
	//	rt[vertex.vBlendIndex.x] = true;
	//	rt[vertex.vBlendIndex.y] = true;
	//	rt[vertex.vBlendIndex.z] = true;
	//	rt[vertex.vBlendIndex.w] = true;
	//}
	return rt;
}
HRESULT CModel::Ready_Meshes()
{
	/* 메시의 갯수를 저장하낟. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	//std::vector<std::thread> Matthreads;
	/* 모델에는 여러개의 머테리얼이 존재하낟. */
	/* 각 머테리얼마다 다양한 재질(Diffuse, Ambient,Specular...)을 텍스쳐로 표현한다. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		//Matthreads.emplace_back(&CModel::Material_Array, this, pModelFilePath, (int)i);
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			/* 얻어온 경로에서 신용할 수 있는 건 파일의 이름과 확장자. */
			aiString		strTexturePath;

			/* 내가 지정한 텍스쳐의 경로를 얻어주낟.*/
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
			_char		szFileName[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szPerfectPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			//MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
			MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);
			
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	/*for (auto& th : Matthreads) {
		if (th.joinable()) {
			th.join();
		}
	}*/

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pNode, _int iParentIndex)
{
	shared_ptr<CBone> pBone = CBone::Create(pNode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iNumChildren = pNode->mNumChildren;

	_int		iNumBones = (_int)m_Bones.size();

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Ready_Bones(pNode->mChildren[i], iNumBones - 1);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		shared_ptr<CAnimation> pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
		m_AnimMapping.emplace(pAnimation->Get_AnimName(), i);
	}

	return S_OK;
}

void CModel::Mesh_Array(shared_ptr<MODEL_DATA> pModelData)
{
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pModelData->Mesh_Datas[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return;

		m_Meshes.emplace_back(pMesh);
	}
}

void CModel::Material_Array(const _char* pModelFilePath, int i)
{
	unique_lock<shared_mutex> lock(m_Mtx);

	MATERIAL_DATA* pMaterial = m_pModelData->Material_Datas[i].get();

	MATERIAL_MESH			MeshMaterial = {};

	for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
	{

		_char		szFullPath[MAX_PATH] = {};
		/* 모델파일경로로부터 뜯어온 문자열. */
		_char		szDrive[MAX_PATH] = {};
		_char		szDir[MAX_PATH] = {};

		/* 모델파일(aiScene)로부터 뜯어온 문자열. */
		_char		szFileName[MAX_PATH] = {};
		_char		szExt[MAX_PATH] = {};

		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		_splitpath_s(pMaterial->szTextureName[j].data(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (szFileName[0] == '\0')
			continue;

		strcpy_s(szFullPath, szDrive);
		strcat_s(szFullPath, szDir);
		strcat_s(szFullPath, szFileName);
		strcat_s(szFullPath, szExt);

		_tchar		szPerfectPath[MAX_PATH] = {};

		MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

		//MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
		MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);

		if (nullptr == MeshMaterial.pTextures[j])
			continue;
	}
	m_Materials.emplace_back(MeshMaterial);

}

vector<shared_ptr<MESH_DATA>> CModel::Make_VoronoiMeshData(_uint iNumCell, const _float3& vScale, vector<_uint>& MeshToPid, vector<vector<int>>& neighbor, 
	const _float3& vCenter, _uint iNumAngle, _uint iNumLayer, FRACTURE_MODE eMode)
{
	_float3 vMax = m_pModelData->VertexInfo.vMax ;
	_float3 vMin = m_pModelData->VertexInfo.vMin;
	XMStoreFloat3(&vMax, XMLoadFloat3(&vMax) / XMLoadFloat3(&vScale));
	XMStoreFloat3(&vMin, XMLoadFloat3(&vMin) / XMLoadFloat3(&vScale));
	if (eMode == CModel::FRACTURE_END)
		eMode = VORONOI;
	hwiVoro::container c{ vMin.x, vMax.x, vMin.y, vMax.y, vMin.z, vMax.z, 6, 6, 6, false,false,false, 8};
	switch (eMode)
	{
	case Engine::CModel::VORONOI:
	{
		for (int i = 0; i < iNumCell; ++i)
		{
			double x = CMath_Manager::Random_Float(vMin.x, vMax.x);
			double y = CMath_Manager::Random_Float(vMin.y, vMax.y);
			double z = CMath_Manager::Random_Float(vMin.z, vMax.z);
			if (c.point_inside(x, y, z))
				c.put(i, x, y, z);
		}
	}
		break;
	case Engine::CModel::SPIDER_WEB:
	{
		_float3 length{ vMax.x - vMin.x , vMax.y - vMin.y ,vMax.z - vMin.z };
		_vector vParticleCenter{ length.x * vCenter.x + vMin.x,
								 length.y * vCenter.y + vMin.y,
								 length.z * vCenter.z + vMin.z, 1.f};
		_uint idx = 0;
		for (_uint i = 0; i < iNumAngle; ++i)
		{
			_float fAngle = (_float)i / (_float)iNumAngle * XM_2PI;
			double nx = cos(fAngle);
			double ny = sin(fAngle);
			for (_uint j = 0; j < iNumLayer; ++j)
			{
				_float fLayer = (_float)(j + 0.5f) / (_float)iNumLayer;
				for (_uint k = 0; k < iNumCell; ++k)
				{
					_float fCell = (_float)(k + 0.5f) / (_float)iNumCell;
					
					double x = nx * length.x * 0.5f * fCell * fCell + vParticleCenter.m128_f32[0];
					double y = ny * length.y * 0.5f * fCell * fCell + vParticleCenter.m128_f32[1];
					double z = fLayer * fLayer * length.z + vMin.z;
					if (c.point_inside(x, y, z))
						c.put(idx, x, y, z);
					++idx;
				}
			}
		}
		iNumCell = idx;
	}
		break;
	case Engine::CModel::HONEYCOMB:
	{
		_float3 length{ vMax.x - vMin.x , vMax.y - vMin.y ,vMax.z - vMin.z };
		_uint idx = 0;
		for (_uint i = 0; i < iNumAngle; ++i)
		{
			_bool even = i % 2;
			double y = (i + 0.5)/(double)(iNumAngle) * length.y + vMin.y;
			for (_uint j = 0; j < iNumLayer; ++j)
			{
				_bool even2 = j % 2;
				double z = (j + 0.5)/(double)(iNumLayer) * length.z + vMin.z;
				for (_uint k = 0; k < iNumCell; ++k)
				{
					double x;
					if (even == even2)
					{
						if (k == 0 )
						{
							x = ((k + 0.001) / (double)iNumCell * length.x) + vMin.x;
							if (c.point_inside(x, y, z))
								c.put(idx, x, y, z);
							++idx;
						}
						x = ((k + 0.999) / (double)iNumCell * length.x) + vMin.x;
					}
					else
					{
						x = ((k + 0.5)/ (double)iNumCell * length.x) + vMin.x;
					}
					if (c.point_inside(x, y, z))
						c.put(idx, x, y, z);
					++idx;
				}
			}
		}
		iNumCell = idx;
	}
		break;
	case Engine::CModel::VOXEL:
	{
		_float3 length{ vMax.x - vMin.x , vMax.y - vMin.y ,vMax.z - vMin.z };
		_uint idx = 0;
		for (_uint i = 0; i < iNumAngle; ++i)
		{
			double y = (i + 0.5) / (double)(iNumAngle)*length.y + vMin.y;
			for (_uint j = 0; j < iNumLayer; ++j)
			{
				double z = (j + 0.5) / (double)(iNumLayer)*length.z + vMin.z;
				for (_uint k = 0; k < iNumCell; ++k)
				{
					double x = ((k + 0.5) / (double)iNumCell * length.x) + vMin.x;
					if (c.point_inside(x, y, z))
						c.put(idx, x, y, z);
					++idx;
				}
			}
		}
		iNumCell = idx;
	}
		break;
	default:
		break;
	}



	hwiVoro::c_loop_all cl(c);
	int idx = 0;

	MESH_VTX_INFO Info = m_pModelData->VertexInfo;
	vector<shared_ptr<MESH_DATA>> MeshDatas(iNumCell, nullptr);
	MeshToPid.resize(iNumCell);
	neighbor.resize(iNumCell);
	if (cl.start()) do
	{
		hwiVoro::CHV_Cell_Neighbor tempCell;
		if (c.compute_cell(tempCell, cl))
		{
			shared_ptr<MESH_DATA> pMeshData;
			double x, y, z;
			cl.pos(x, y, z);

			//c.apply_walls(tempCell, x, y, z);
			tempCell.neighbors(neighbor[cl.pid()]);
			GAMEINSTANCE->Voronoi_CellToMeshData(tempCell, pMeshData, _float3(x, y, z), string("Voronoi_") + to_string(cl.pid()), cl.pid(), vScale, Info);
			MeshDatas[cl.pid()] = (pMeshData);
			MeshToPid[cl.pid()] = (cl.pid());
		}
		++idx;
	} while (cl.inc());
	return MeshDatas;
}

void CModel::Do_Intersect(_uint iNumCell, const vector<_uint>& Pid, const vector<shared_ptr<MESH_DATA>>& MeshDatas, vector<shared_ptr<MODEL_DATA>>& out)
{
	auto pTimer = CTimer::Create();
	pTimer->Update();
	vector<unique_ptr<BSP_Node>> CellNodes(MeshDatas.size());
	vector<CSG_Mesh> CellMeshes(MeshDatas.size());
	_uint idx = 0;
	for (auto& pMeshData : MeshDatas)
	{
		CellMeshes[idx] = CCSG_Manager::ConvertMeshData(*pMeshData, 1.f);
		vector<_uint3> triangels;
		for (_uint iTri = 0; iTri < CellMeshes[idx].indices.size(); iTri += 3)
		{
			triangels.push_back(_uint3(CellMeshes[idx].indices[iTri], CellMeshes[idx].indices[iTri + 1], CellMeshes[idx].indices[iTri + 2]));
		}
		CellNodes[idx] = (CCSG_Manager::BuildBSPTree(CellMeshes[idx], triangels, CellMeshes[idx].center));
		++idx;
	}


	vector<unique_ptr<BSP_Node>> OriNodes(m_iNumMeshes);
	vector<CSG_Mesh> OriMeshes(m_iNumMeshes);
	idx = 0;
	for (auto& pMeshData : m_pModelData->Mesh_Datas)
	{
		OriMeshes[idx] = CCSG_Manager::ConvertMeshData(*pMeshData);
		vector<_uint3> triangels;
		for (_uint iTri = 0; iTri < OriMeshes[idx].indices.size(); iTri += 3)
		{
			triangels.push_back(_uint3(OriMeshes[idx].indices[iTri], OriMeshes[idx].indices[iTri + 1], OriMeshes[idx].indices[iTri + 2]));
		}
		OriNodes[idx] = (CCSG_Manager::BuildBSPTree(OriMeshes[idx], triangels, OriMeshes[idx].center));
		++idx;
	}
	pTimer->Update();
	cout << "Node Build Time : " << pTimer->Get_TimeDelta() << "\n";

	for (_uint iMesh = 0; iMesh < m_iNumMeshes; ++iMesh)
	{
		out.push_back(make_shared<MODEL_DATA>());
		vector<shared_ptr<MESH_DATA>> ObjectiveDatas(CellNodes.size());
		vector<thread> threads(10);
		_uint iteration = 0;
#ifdef _DEBUG
		_uint NumThreads = 1;
#else
		_uint NumThreads = 20;
#endif
		for (_uint iT = 0; iT < (iNumCell + NumThreads - 1) / NumThreads; ++iT)
		{
			for (_uint iThread = 0; iThread < NumThreads; ++iThread)
			{
				if (iteration >= CellNodes.size())
					break;
				threads.emplace_back(&CModel::Intersect, this, std::ref(OriNodes[iMesh]), std::ref(CellNodes[iteration]), std::ref(ObjectiveDatas[iteration]), iteration);
				++iteration;
			}
			for (auto& th : threads) {
				if (th.joinable()) {
					th.join();
				}
			}
		}
		for (auto& th : threads) {
			if (th.joinable()) {
				th.join();
			}
		}
		threads.clear();
		vector<shared_ptr<BONE_DATA>> BD;
		auto pRB = make_shared<BONE_DATA>();
		pRB->OffsetMatrix = Identity;
		pRB->szName = "RootNode";
		BD.push_back(pRB);
		for (auto i = ObjectiveDatas.begin(); i != ObjectiveDatas.end();)
		{
			if (!(*i))
			{
				i = ObjectiveDatas.erase(i);
				continue;
			}
			(*i)->szName = string("Voronoi_") + to_string(Pid[i - ObjectiveDatas.begin()]) + "_" + to_string(iMesh);
			//->Bone_Datas
			for (_uint j = 0; j < (*i)->iNumVertices; ++j)
			{
				(*i)->pAnimVertices[j].vBlendIndex.x = i - ObjectiveDatas.begin() + 1;
				(*i)->pAnimVertices[j].vBlendWeight.x = 1.f;
			}
			auto pBD = make_shared<BONE_DATA>();
			pBD->OffsetMatrix = Identity;
			pBD->szName = (*i)->szName;
			BD.push_back(pBD);
			++i;
			//ObjectiveDatas[i]->Bone_Datas.push_back(pBD);
		}
		for (auto& od : ObjectiveDatas)
		{
			od->Bone_Datas.insert(od->Bone_Datas.end(), BD.begin(), BD.end());
			od->iNumBones = od->Bone_Datas.size();
		}

		out.back()->Mesh_Datas.insert(out.back()->Mesh_Datas.end(), ObjectiveDatas.begin(), ObjectiveDatas.end());
	}
}



shared_ptr<CModel> CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	struct MakeSharedEnabler : public CModel
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CModel(pDevice, pContext) { }
	};

	shared_ptr<CModel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CModel::Clone(void* pArg)
{

	struct MakeSharedEnabler : public CModel
	{
		MakeSharedEnabler(const CModel& rhs) : CModel(rhs) { }
	};

	shared_ptr<CModel> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModel"));
		assert(false);
	}

	return pInstance;
}

vector<shared_ptr<MODEL_DATA>> CModel::Do_Voronoi(_uint iNumCell, const _float3& vScale, vector<vector<int>>& neighbor,
	const _float3& vCenter, _uint iNumAngle , _uint iNumLayer, FRACTURE_MODE eMode)
{
	auto pTimer = CTimer::Create();
	
	vector<shared_ptr<MODEL_DATA>> rt{};
	if (m_eModelType == MODEL_TYPE::ANIM)
	{
		return rt;
	}
	vector<_uint> MeshToPid;
	pTimer->Update();
	vector<shared_ptr<MESH_DATA>> MeshDatas = Make_VoronoiMeshData(iNumCell, vScale, MeshToPid, neighbor, vCenter, iNumAngle, iNumLayer, eMode);
	pTimer->Update();
	cout << "Make_Voronoi : " << pTimer->Get_TimeDelta() <<'\n';
	switch (eMode)
	{
	case Engine::CModel::VORONOI:
		break;
	case Engine::CModel::SPIDER_WEB:
		iNumCell = iNumCell * iNumAngle * iNumLayer;
		break;
	case Engine::CModel::HONEYCOMB:
		iNumCell = iNumCell * iNumAngle * iNumLayer;
		break;
	case Engine::CModel::VOXEL:
		iNumCell = iNumCell * iNumAngle * iNumLayer;
		break;
	case Engine::CModel::FRACTURE_END:
		break;
	default:
		break;
	}

	Do_Intersect(iNumCell, MeshToPid, MeshDatas, rt);
	pTimer->Update();
	cout << "Do_Intersect : " << pTimer->Get_TimeDelta() << '\n';

	_uint idx = 0;
	_char szDir[MAX_PATH] = {};
	_char szFileName[MAX_PATH] = {};
	_splitpath_s(m_pModelData->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	for (auto& md : rt)
	{
		md->RootNode = make_shared<NODE_DATA>();
		md->RootNode->iNumChildren = md->Mesh_Datas.size();
		md->RootNode->szName = "RootNode";
		md->RootNode->TransformationMatrix = Identity;
		for (_uint i = 0; i < md->RootNode->iNumChildren; ++i)
		{
			md->RootNode->pChildren.push_back(make_shared<NODE_DATA>());
			md->RootNode->pChildren[i]->szName = md->Mesh_Datas[i]->szName;
			md->RootNode->pChildren[i]->iNumChildren = 0;
			md->RootNode->pChildren[i]->TransformationMatrix = Identity;
		}
		md->szModelFileName = m_pModelData->szModelFileName + string("_") + to_string(idx);
		md->szModelFilePath = string(szDir) + md->szModelFileName + string(".bin");
		md->eModelType = m_eModelType;

		md->iNumMeshs = md->Mesh_Datas.size();
		md->eModelType = MODEL_TYPE::ANIM;
		md->TransformMatrix = m_TransformMatrix;
		auto pMaterial = make_shared<MATERIAL_DATA>();
		_uint texIdx = 0;
		for (auto& texName : pMaterial->szTextureName)
		{
			texName = m_pModelData->Material_Datas[idx]->szTextureName[texIdx];
			++texIdx;
		}
		md->Material_Datas.push_back(pMaterial);
		auto pInMat = make_shared<MATERIAL_DATA>();
		pInMat->szTextureName[aiTextureType_DIFFUSE] = "In_D.png";
		pInMat->szTextureName[aiTextureType_NORMALS] = "In_N.png";
		md->Material_Datas.push_back(pInMat);
		md->iNumMaterials = 2;
		++idx;
	}
	pTimer->Update();
	cout << "Rest : " << pTimer->Get_TimeDelta() << '\n';

	return rt;
}

vector<shared_ptr<MODEL_DATA>> CModel::Blast_Voronoi(_uint iNumCell, const _float3& vScale, vector<vector<int>>& neighbor, const _float3& vCenter, _uint iNumAngle, _uint iNumLayer, FRACTURE_MODE eMode)
{
	vector<shared_ptr<MODEL_DATA>> rt{};
	auto pBlastManager = CBlastManager::Create();
	neighbor.resize(iNumCell);
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		auto& pMeshData = m_pModelData->Mesh_Datas[i];
		
		if (pMeshData->iNumVertices == 0)
			continue;
		auto pBlastMesh = pBlastManager->ConvertMeshData(*pMeshData, 1.f);
		//pBlastManager->CleanseMesh(pBlastMesh);
		auto vecDebris = pBlastManager->Voronoi_Fracture(pBlastMesh, iNumCell);
		vector<shared_ptr<BONE_DATA>> BD;
		auto pRB = make_shared<BONE_DATA>();
		pRB->OffsetMatrix = Identity;
		pRB->szName = "RootNode";
		BD.push_back(pRB);
		rt.push_back(make_shared<MODEL_DATA>());
		for (auto iter = vecDebris.begin(); iter != vecDebris.end(); ++iter)
		{
			(*iter)->szName = string("Voronoi_") + to_string(i) + "_" + to_string(iter - vecDebris.begin());
			for (_uint k = 0; k < (*iter)->iNumVertices; ++k)
			{
				(*iter)->pAnimVertices[k].vBlendIndex.x = iter - vecDebris.begin() + 1;
				(*iter)->pAnimVertices[k].vBlendWeight.x = 1.f;
			}
			auto pBD = make_shared<BONE_DATA>();
			pBD->OffsetMatrix = Identity;
			pBD->szName = (*iter)->szName;
			BD.push_back(pBD);
		}
		for (auto& od : vecDebris)
		{
			od->Bone_Datas.insert(od->Bone_Datas.end(), BD.begin(), BD.end());
			od->iNumBones = od->Bone_Datas.size();
		}

		rt.back()->Mesh_Datas.insert(rt.back()->Mesh_Datas.end(), vecDebris.begin(), vecDebris.end());
	}
	_uint idx = 0;
	_char szDir[MAX_PATH] = {};
	_char szFileName[MAX_PATH] = {};
	_splitpath_s(m_pModelData->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	for (auto& md : rt)
	{
		md->RootNode = make_shared<NODE_DATA>();
		md->RootNode->iNumChildren = md->Mesh_Datas.size();
		md->RootNode->szName = "RootNode";
		md->RootNode->TransformationMatrix = Identity;
		for (_uint i = 0; i < md->RootNode->iNumChildren; ++i)
		{
			md->RootNode->pChildren.push_back(make_shared<NODE_DATA>());
			md->RootNode->pChildren[i]->szName = md->Mesh_Datas[i]->szName;
			md->RootNode->pChildren[i]->iNumChildren = 0;
			md->RootNode->pChildren[i]->TransformationMatrix = Identity;
		}
		md->szModelFileName = m_pModelData->szModelFileName + string("_") + to_string(idx);
		md->szModelFilePath = string(szDir) + md->szModelFileName + string(".bin");
		md->eModelType = m_eModelType;

		md->iNumMeshs = md->Mesh_Datas.size();
		md->eModelType = MODEL_TYPE::ANIM;
		md->TransformMatrix = m_TransformMatrix;
		auto pMaterial = make_shared<MATERIAL_DATA>();
		_uint texIdx = 0;
		for (auto& texName : pMaterial->szTextureName)
		{
			texName = m_pModelData->Material_Datas[idx]->szTextureName[texIdx];
			++texIdx;
		}
		md->Material_Datas.push_back(pMaterial);
		auto pInMat = make_shared<MATERIAL_DATA>();
		pInMat->szTextureName[aiTextureType_DIFFUSE] = "In_D.png";
		pInMat->szTextureName[aiTextureType_NORMALS] = "In_N.png";
		md->Material_Datas.push_back(pInMat);
		md->iNumMaterials = 2;
		++idx;
	}
	return rt;
}

vector<shared_ptr<MODEL_DATA>> CModel::Blast_Voronoi(_uint iNumCell)
{
	vector<shared_ptr<MODEL_DATA>> rt{};
	auto pBlastManager = CBlastManager::Create();
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		auto& pMeshData = m_pModelData->Mesh_Datas[i];

		if (pMeshData->iNumVertices == 0)
			continue;
		auto pBlastMesh = pBlastManager->ConvertMeshData(*pMeshData, 1.f);
		//pBlastManager->CleanseMesh(pBlastMesh);
		auto vecDebris = pBlastManager->Voronoi_Fracture(pBlastMesh, iNumCell, false);
		rt.push_back(make_shared<MODEL_DATA>());
		for (auto iter = vecDebris.begin(); iter != vecDebris.end(); ++iter)
		{
			(*iter)->szName = string("Voronoi_") + to_string(i) + "_" + to_string(iter - vecDebris.begin());
		}
		rt.back()->Mesh_Datas.insert(rt.back()->Mesh_Datas.end(), vecDebris.begin(), vecDebris.end());
	}
	_uint idx = 0;
	_char szDir[MAX_PATH] = {};
	_char szFileName[MAX_PATH] = {};
	_splitpath_s(m_pModelData->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	for (auto& md : rt)
	{
		md->szModelFileName = m_pModelData->szModelFileName + string("_") + to_string(idx);
		md->szModelFilePath = string(szDir) + md->szModelFileName + string(".bin");
		md->iNumMeshs = md->Mesh_Datas.size();
		md->eModelType = MODEL_TYPE::NONANIM;
		md->TransformMatrix = m_TransformMatrix;
		auto pMaterial = make_shared<MATERIAL_DATA>();
		_uint texIdx = 0;
		for (auto& texName : pMaterial->szTextureName)
		{
			texName = m_pModelData->Material_Datas[idx]->szTextureName[texIdx];
			++texIdx;
		}
		md->Material_Datas.push_back(pMaterial);
		auto pInMat = make_shared<MATERIAL_DATA>();
		pInMat->szTextureName[aiTextureType_DIFFUSE] = "In_D.png";
		pInMat->szTextureName[aiTextureType_NORMALS] = "In_N.png";
		md->Material_Datas.push_back(pInMat);
		md->iNumMaterials = 2;
		++idx;
	}
	return rt;
}

shared_ptr<CModel> CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, shared_ptr<MODEL_DATA> pModelData, _fmatrix TransformMatrix)
{
	struct MakeSharedEnabler : public CModel
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CModel(pDevice, pContext) {}
	};
	shared_ptr<CModel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	XMStoreFloat4x4(&pInstance->m_TransformMatrix, TransformMatrix);
	pInstance->m_pModelData = pModelData;
	pInstance->Init_Model_Internal(pModelData, "", 0);

	return pInstance;
}

void CModel::Intersect(unique_ptr<class BSP_Node>& ori, unique_ptr<BSP_Node>& cell , shared_ptr<MESH_DATA>& out, _uint iBlendID)
{
	auto pOriClone = ori->Clone();
	CCSG_Manager::CSG_Intersect(pOriClone, cell);
	CSG_Mesh Mesh;
	CCSG_Manager::ReconstructMesh(Mesh, pOriClone);
	Mesh.Optimize();
	//(Mesh.indices.size() != 0);
	
	out = CCSG_Manager::RevertMeshData(Mesh, true, iBlendID);
}

void CModel::Free()
{

}

HRESULT CModel::Create_Materials(const _char* pModelFilePath)
{

	vector<thread> Matthreads;

	m_iNumMaterials = m_pModelData->iNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MATERIAL_DATA* pMaterial = m_pModelData->Material_Datas[i].get();

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{

			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
			_char		szFileName[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};
			_char		strTga[] = ".tga";

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(pMaterial->szTextureName[j].data(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			if (szFileName[0] == '\0')
				continue;
			if (strcmp(szExt, strTga) == false) {
				strcpy_s(szExt, ".dds");
			}


			_tchar		szPerfectPath[MAX_PATH] = {};

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			// ALBM -> ATOS
			if (j == 2)	
			{
				string strTarget = szFileName;
				string toReplace = "ALBM";
				string replacement = "ATOS_NoesisAO";

				size_t pos = strTarget.find(toReplace);
				if (pos != std::string::npos)
					strTarget.replace(pos, toReplace.length(), replacement);
				strcat_s(szFullPath, strTarget.c_str());
				strcat_s(szFullPath, szExt);

				MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);
				_bool isExist = filesystem::exists(szPerfectPath);
				if (!isExist)
				{
					ZeroMemory(szPerfectPath, sizeof(szPerfectPath));
					_char szTempPath[MAX_PATH] = "../Bin/Resources/Textures/NullWhite.png";
					MultiByteToWideChar(CP_ACP, 0, szTempPath, (_int)strlen(szTempPath), szPerfectPath, MAX_PATH);
					//continue;
				}
			}
			else
			{
				strcat_s(szFullPath, szFileName);
				strcat_s(szFullPath, szExt);

				MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);
			}

			MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	for (auto& th : Matthreads) {
		if (th.joinable()) {
			th.join();
		}
	}

	return S_OK;
}

HRESULT CModel::Bind_Buffers(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_Buffer();
}


HRESULT CModel::Create_Animations()
{
	m_iNumAnimations = m_pModelData->iNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		shared_ptr<CAnimation> pAnimation = CAnimation::Create(m_pModelData->Animation_Datas[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
		m_AnimMapping.emplace(pAnimation->Get_AnimName(), i);
	}

	return S_OK;
}

HRESULT CModel::Create_Bone(shared_ptr<NODE_DATA> pNode, _int iParentIndex)
{
	shared_ptr<CBone> pBone = CBone::Create(pNode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iNumChildren = pNode->iNumChildren;

	_int		iNumBones = (_int)m_Bones.size();

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Create_Bone(pNode->pChildren[i], iNumBones - 1);
	}

	return S_OK;


}
