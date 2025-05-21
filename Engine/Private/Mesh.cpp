#include "..\Public\Mesh.h"
#include "GameInstance.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer{ rhs }
{
}

CMesh::~CMesh()
{
	Free();
}

void CMesh::Cooking_TriangleMesh()
{
	PxTriangleMeshDesc meshDesc;
	PxCookingParams params = PxCookingParams(PxTolerancesScale());

	meshDesc.points.count = static_cast<PxU32>(vertices.size());
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices.data();

	if (meshDesc.isValid() == false)
		return;


	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	/* Repair */
	m_pTriangleMesh = CGameInstance::Get_Instance()->Get_Physics()->createTriangleMesh(readBuffer);
}

void CMesh::Cooking_ConvexMesh()
{
	PxConvexMeshDesc meshDesc;
	PxCookingParams params = PxCookingParams(PxTolerancesScale());

	meshDesc.points.count = static_cast<PxU32>(vertices.size());
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();
	meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION;
	
	meshDesc.indices.count = static_cast<PxU32>(indices.size());
	meshDesc.indices.stride = sizeof(PxU32);
	meshDesc.indices.data = indices.data();

	if (meshDesc.isValid() == false)
		return;


	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;
	PxCookConvexMesh(params, meshDesc, writeBuffer, &result);
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	/* Repair */
	m_pConvexMesh = CGameInstance::Get_Instance()->Get_Physics()->createConvexMesh(readBuffer);
}

void CMesh::Cooking_ConvexMesh(const PxVec3& scale)
{
	auto tempvert = vertices;
	for (auto& vert : tempvert)
	{
		vert = vert.multiply(scale);
	}
	PxConvexMeshDesc meshDesc;
	PxCookingParams params = PxCookingParams(PxTolerancesScale());

	meshDesc.points.count = static_cast<PxU32>(tempvert.size());
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = tempvert.data();
	meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION;

	meshDesc.indices.count = static_cast<PxU32>(indices.size());
	meshDesc.indices.stride = sizeof(PxU32);
	meshDesc.indices.data = indices.data();

	if (meshDesc.isValid() == false)
		return;


	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;
	PxCookConvexMesh(params, meshDesc, writeBuffer, &result);
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	/* Repair */
	m_pConvexMesh = CGameInstance::Get_Instance()->Get_Physics()->createConvexMesh(readBuffer);
}

const PxVec3& CMesh::ComputeCenter()
{
	
	for (auto& point : vertices)
	{
		m_vCenter += point;
	}
	m_vCenter /= vertices.size();

	return m_vCenter;
}

HRESULT CMesh::Initialize_Prototype(MODEL_TYPE eModelType, const aiMesh* pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iIndexStride = 4;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT	hr = eModelType == MODEL_TYPE::NONANIM ? Ready_VIBuffer_For_NonAnim(pAIMesh, TransformMatrix) : Ready_VIBuffer_For_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	/* 삼각형당 루프를 돈다. */
	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];

	}
	for (unsigned int i = 0; i < pAIMesh->mNumFaces; ++i) {
		const aiFace& face = pAIMesh->mFaces[i];
		if (face.mNumIndices != 3) continue; // 삼각형이 아닌 페이스는 무시
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Cooking_TriangleMesh();
	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype(MODEL_TYPE eModelType, shared_ptr<MESH_DATA> pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->szName.c_str());
	m_iMaterialIndex = pAIMesh->iMaterialIndex;


	m_iIndexStride = 4;
	m_iNumVertices = pAIMesh->iNumVertices;
	m_iNumIndices = pAIMesh->iNumFaces * 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT	hr = eModelType == MODEL_TYPE::NONANIM ? Ready_VIBuffer_For_NonAnim(pAIMesh, TransformMatrix) : Ready_VIBuffer_For_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->iNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->pIndices[i]._1;
		pIndices[iNumIndices++] = pAIMesh->pIndices[i]._2;
		pIndices[iNumIndices++] = pAIMesh->pIndices[i]._3;

		const FACEINDICES32& face = pAIMesh->pIndices[i];
		//if (face.numIndices != 3) continue;

		indices.push_back(face._1);
		indices.push_back(face._2);
		indices.push_back(face._3);

	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	//Cooking_TriangleMesh();
	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4* pMatrices, const vector<shared_ptr<class CBone>>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&pMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_Bones[i]]->Get_CombinedTransformationMatrix());
	}
}


HRESULT CMesh::Ready_VIBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix TransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	/* 생성하고자하는 버퍼의 속성을 설정하낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 생성하고자하는 버퍼의 초기값을 설정한다. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), TransformMatrix));

		vertices.push_back(PxVec3(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);



	return S_OK;
}

HRESULT CMesh::Ready_VIBuffer_For_Anim(const aiMesh* pAIMesh, vector<shared_ptr<class CBone>>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	/* 생성하고자하는 버퍼의 속성을 설정하낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 생성하고자하는 버퍼의 초기값을 설정한다. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	/* 이 메시에 영향을 주는 뼈의 갯수. */
	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* 이 메시에 영향을 주는 ㅃㅕ들 중 i번째 뼈. */
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4		OffsetMatrix{};
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		_uint			iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
			{
				if (false == strcmp(pAIBone->mName.data, pBone->Get_Name()))
					return true;

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		m_Bones.emplace_back(iBoneIndex);

		/* 이 뼈는 몇개의 정점에게 영향을 주는데? */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* pAIBone->mWeights[j].mVertexId : 이 뼈가 j번째 영향을 주는 정점의 인덱스 */
			if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		/* 이 메시랑 이름이 똑같은 뼈를 찾아서 푸쉬해놓자. */
		_uint			iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
			{
				if (false == strcmp(m_szName, pBone->Get_Name()))
					return true;

				++iBoneIndex;

				return false;
			});

		/* 내 메시랑 이름이 같은 뼈의 인덱스(전체뼈중의)를 메시안에 보관한다. */
		m_Bones.emplace_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VIBuffer_For_NonAnim(shared_ptr<MESH_DATA> pAIMesh, _fmatrix TransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	/* 생성하고자하는 버퍼의 속성을 설정하낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 생성하고자하는 버퍼의 초기값을 설정한다. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->pVertices[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->pVertices[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->pVertices[i].vTexcoord, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->pVertices[i].vTangent, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), TransformMatrix));

		vertices.push_back(PxVec3(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);



	return S_OK;
}

HRESULT CMesh::Ready_VIBuffer_For_Anim(shared_ptr<MESH_DATA> pAIMesh, vector<shared_ptr<class CBone>>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	/* 생성하고자하는 버퍼의 속성을 설정하낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 생성하고자하는 버퍼의 초기값을 설정한다. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->pAnimVertices[i].vPosition, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->pAnimVertices[i].vNormal, sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->pAnimVertices[i].vTexcoord, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->pAnimVertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndices, &pAIMesh->pAnimVertices[i].vBlendIndex, sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeights, &pAIMesh->pAnimVertices[i].vBlendWeight, sizeof(_float4));

		vertices.push_back(PxVec3(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z));
	}

	m_iNumBones = pAIMesh->iNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		shared_ptr<BONE_DATA> pAIBone = pAIMesh->Bone_Datas[i];

		_float4x4		OffsetMatrix{};
		memcpy(&OffsetMatrix, &pAIBone->OffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		_uint			iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
			{
				if (false == strcmp(pAIBone->szName.c_str(), pBone->Get_Name()))
					return true;

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		m_Bones.emplace_back(iBoneIndex);

	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		/* 이 메시랑 이름이 똑같은 뼈를 찾아서 푸쉬해놓자. */
		_uint			iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
			{
				if (false == strcmp(m_szName, pBone->Get_Name()))
					return true;

				++iBoneIndex;

				return false;
			});

		/* 내 메시랑 이름이 같은 뼈의 인덱스(전체뼈중의)를 메시안에 보관한다. */
		m_Bones.emplace_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

shared_ptr<CMesh> CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eModelType, const aiMesh* pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix)
{
	struct MakeSharedEnabler : public CMesh
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMesh(pDevice, pContext) { }
	};

	shared_ptr<CMesh> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, Bones, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CMesh> CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eModelType, shared_ptr<MESH_DATA> pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix)
{

	struct MakeSharedEnabler : public CMesh
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMesh(pDevice, pContext) { }
	};

	shared_ptr<CMesh> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, Bones, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CMesh::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CMesh
	{
		MakeSharedEnabler(const CMesh& rhs) : CMesh(rhs) { }
	};

	shared_ptr<CMesh> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		assert(false);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();
}
