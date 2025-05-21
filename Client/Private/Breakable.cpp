#include "Breakable.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "DynamicCollider.h"
#include "GameManager.h"
#include "Mesh.h"
#include "CSG_Manager.h"
#include "iostream"
#include "StaticCollider.h"
#include "Imgui_Manager.h"
#include "Math_Manager.h"
#include "NvBlastExtAuthoring.h"
#include "NvBlastExtAuthoringConvexMeshBuilder.h"
#include "NvBlastExtAuthoringBondGenerator.h"
#include "NvBlast.h"
#include "NvBlastTk.h"
#include "NvBlastExtAuthoringMeshCleaner.h"

wstring CBreakable::ObjID = TEXT("CBreakable");

CBreakable::CBreakable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBreakable::CBreakable(const CBreakable& rhs)
	: CGameObject(rhs)
{
}

CBreakable::~CBreakable()
{
	Free();
}

HRESULT CBreakable::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Cube"), TEXT("../Bin/Resources/Models/Cube/Cube.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																					   0.f,	 0.01f, 0.f,  0.f,
																																					   0.f,  0.f,  0.01f, 0.f,
																																					   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Sphere"), TEXT("../Bin/Resources/Models/Cube/Sphere.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Monkey"), TEXT("../Bin/Resources/Models/Cube/Monkey.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Cube_Cut"), TEXT("../Bin/Resources/Models/Cube/Cube_Cut.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Torus"), TEXT("../Bin/Resources/Models/Cube/Torus.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																						   0.f,	 0.01f, 0.f,  0.f,
																																						   0.f,  0.f,  0.01f, 0.f,
																																						   0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CBreakable::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;
		m_fTorqueThreshold = ((FDO_DESC*)pArg)->fTorqueThreshold;
		m_fForceThreshold = ((FDO_DESC*)pArg)->fForceThreshold;
		m_iNumCell = ((FDO_DESC*)pArg)->iNumCell;
		m_strInD = ((FDO_DESC*)pArg)->strInD;
		m_strInN = ((FDO_DESC*)pArg)->strInN;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;
	


	Fracture_Root(m_iNumCell, 3);
	
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_OldTime);
	QueryPerformanceCounter(&m_OriginTime);

	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}


void CBreakable::PriorityTick(_float fTimeDelta)
{
	if (m_bOnContact)
	{
		Insert_Nodes();
	}
	if (m_bDirty)
	{
		CheckIsland();
	}
	m_bDirty = false;
	m_bOnContact = false;
}

void CBreakable::Tick(_float fTimeDelta)
{
	_float3 scale = m_pTransformCom->Get_Scale();
	//if(m_bContact)
	//{
	//	for (auto& node : m_Nodes)
	//	{
	//		if (node.second->pActor)
	//		{
	//			PxMat44 PxMat(node.second->pActor->getGlobalPose());
	//			XMStoreFloat4x4(&node.second->Transform, XMMatrixScalingFromVector(XMLoadFloat3(&scale)) * PxMatToXMMatrix(PxMat));
	//		}
	//	}
	//}
	//else
	//{
	//	m_pDynamicColliderCom->Update(fTimeDelta);
	//	for (auto& node : m_Nodes)
	//	{
	//		if (node.second->pActor)
	//		{
	//			XMStoreFloat4x4(&node.second->Transform, m_pTransformCom->Get_WorldMatrix_XMMat());
	//		}
	//	}
	//}

	for (auto& actor : m_Actors)
	{
		PxMat44 pxMat{ actor.pRigidBody->getGlobalPose() };
		_matrix World = XMMatrixScalingFromVector(XMLoadFloat3(&scale)) * PxMatToXMMatrix(pxMat);
		XMStoreFloat4x4(&actor.m_Transform, World);
		for (_uint i : actor.Nodes)
		{
			if(m_Nodes.count(i))
				XMStoreFloat4x4(&m_Nodes[i]->Transform, World);
		}
	}

}

void CBreakable::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CBreakable::Render()
{
	Render_Static();
	return S_OK;
}

void CBreakable::OnPxContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPair, CGameObject* pOther)
{
	PxContactPairPoint* pair = new PxContactPairPoint[2];
	
	auto iter0 = find_if(m_Actors.begin(), m_Actors.end(), [&](const Actor& lhs) {return lhs.pRigidBody == pairHeader.actors[0]; });
	auto iter1 = find_if(m_Actors.begin(), m_Actors.end(), [&](const Actor& lhs) {return lhs.pRigidBody == pairHeader.actors[1]; });
	_int iActor0Idx = iter0 != m_Actors.end() ? iter0 - m_Actors.begin() : -1;
	_int iActor1Idx = iter1 != m_Actors.end() ? iter1 - m_Actors.begin() : -1;

	_uint rt = pairs[nbPair - 1].extractContacts(pair, 2);
	if(rt == 0)
	{
		delete[] pair;
		return;
	}
	DamageFunction(pair[rt - 1], iActor0Idx, iActor1Idx);

	delete[] pair;
}

void CBreakable::Apply_Damage_Shape(const PxGeometry& geo, const PxTransform& pose)
{
	for (auto& elem0 : m_Edges)
	{
		for (auto& elem1 : elem0)
		{
			if (elem1.second.fHealth < 0.f)
				continue;
			_vector vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&elem1.second.vCenter), XMLoadFloat4x4(&m_Actors[elem1.second.iActorIdx].m_Transform));
			PxVec3 pxPos{vWorldPos.m128_f32[0],vWorldPos.m128_f32[1] ,vWorldPos.m128_f32[2] };
			_float fDist = PxGeometryQuery::pointDistance(pxPos, geo, pose);

			if (fDist < 0.001f)
			{
				elem1.second.fHealth = -1.f;
				m_bDirty = true;
				m_Actors[elem1.second.iActorIdx].pRigidBody->addForce(PxVec3{CMath_Manager::Random_Float(-100.f, 100.f),
					CMath_Manager::Random_Float(-100.f, 100.f),CMath_Manager::Random_Float(-100.f, 100.f) }, PxForceMode::eACCELERATION);
			}
		}
	}
	;
}

void CBreakable::Apply_Inner_Texture(const string& szFilename, shared_ptr<CTexture> pTexture, aiTextureType eType)
{
	if (!m_pModelCom)
		return;
	m_pModelCom->Get_ModelData()->Material_Datas[1]->szTextureName[eType] = szFilename;
	m_pModelCom->Chander_Material(1, pTexture, eType);
}


HRESULT CBreakable::Render_Static()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;


	if (auto pInD = GAMEMANAGER->Find_GlobalTexture(m_strInD))
		pInD->Bind_ShaderResource(m_pShaderCom, "g_Texture_In");
	else
		CGame_Manager::Get_Instance()->Get_InDiffuse()->Bind_ShaderResource(m_pShaderCom, "g_Texture_In");

	if (auto pInD = GAMEMANAGER->Find_GlobalTexture(m_strInN))
		pInD->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture_In");
	else
		CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture_In");
	
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
	{
		CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
	}
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
	{
		CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
	}

	for (auto& pNode : m_Nodes)
	{
		if (pNode.second->isLeaf)
		{

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &pNode.second->Transform)))
				return E_FAIL;
			m_pShaderCom->Begin((_uint)MESH_PASS::Slice);
			m_Meshes[pNode.second->MeshIdx]->Render();
		}
	}


	return S_OK;
}

void CBreakable::PerformanceCounter()
{
	QueryPerformanceCounter(&m_CurTime);

	if (m_CurTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurTime;
	}

	m_fTimeDelta = float(m_CurTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	m_OldTime = m_CurTime;
}

void CBreakable::Fracture(_uint iMaxChild, _uint iMaxDepth)
{
	Fracture_Root(iMaxChild, iMaxDepth);
}

void CBreakable::Fracture_Root(_uint iMaxChild, _uint iMaxDepth)
{
	m_Actors.clear();
	auto pBlastManager = CBlastManager::Create();

	vector<BlastMesh*> Meshes;
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		auto pMesh = pBlastManager->ConvertMeshData(*m_pModelCom->Get_ModelData()->Mesh_Datas[i], 0.f);
		Meshes.push_back(pMesh);
	}

	//auto MDS = m_pModelCom->Blast_Voronoi(iMaxChild);
	//m_pModelData = MDS[0];
	//vector<shared_ptr<CBone>> Bone{};
	//CMesh::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, MDS[0]->Mesh_Datas[0], Bone, XMLoadFloat4x4(&MDS[0]->TransformMatrix));

	if (m_pFractureTool)
		m_pFractureTool->release();
	m_pFractureTool = NvBlastExtAuthoringCreateFractureTool();
	auto pClener = NvBlastExtAuthoringCreateMeshCleaner();
	for (auto& mesh : Meshes)
	{
		if (m_pFractureTool->isMeshContainOpenEdges(mesh))
		{
			mesh = pClener->cleanMesh(mesh);
		}
	}
	delete pClener;
	m_pFractureTool->setSourceMeshes(Meshes.data(), Meshes.size());

	for(_uint i = 0; i < Meshes.size(); ++i)
	{
		auto voronoi = NvBlastExtAuthoringCreateVoronoiSitesGenerator(Meshes[i], &randomGen);
		voronoi->uniformlyGenerateSitesInMesh(iMaxChild / Meshes.size());
		const NvcVec3* sites = nullptr;
		uint32_t numSite = voronoi->getVoronoiSites(sites);
		_vector scale = { 1.f / (m_pTransformCom->Get_Scale()).x,
			1.f / (m_pTransformCom->Get_Scale()).y,
			1.f / (m_pTransformCom->Get_Scale()).z, 0.f };
		scale = XMVector3Normalize(scale);
		NvcVec3 vScale = { XMVectorGetX(scale), XMVectorGetY(scale), XMVectorGetZ(scale) };

		NvcQuat qRot{0,0,0,1};
		m_pFractureTool->voronoiFracturing(i, numSite, sites, vScale, qRot, false);

		delete voronoi;
	}

	m_pFractureTool->finalizeFracturing();
	auto pBondGen = NvBlastExtAuthoringCreateBondGenerator(nullptr);
	_bool* isSuporting = new _bool[m_iNumCell];
	for (_uint i = 0; i < m_iNumCell; ++i)
	{
		isSuporting[i] = true;
	}
	NvBlastBondDesc* desc;
	NvBlastChunkDesc* chunkDesc;
	iNumBond = pBondGen->buildDescFromInternalFracture(m_pFractureTool, isSuporting, desc, chunkDesc);
	
	//NvBlastAssetDesc assetDesc;
	//assetDesc.chunkDescs = chunkDesc;
	//assetDesc.chunkCount = m_iNumCell;
	//assetDesc.bondDescs = desc;
	//assetDesc.bondCount = iNumBond;
	//Nv::Blast::TkFramework* pTkFramework = NvBlastTkFrameworkCreate();
	//pTkFramework->createAsset();
	//Nv::Blast::TkActor* pActor = pTkFramework->createActor(assetDesc, nullptr, 0);
	//pActor->
	Build_Hierarchy();
	m_Edges = vector<map<_uint, chunkEdge>>(m_iNumCell + 1, map<_uint, chunkEdge>{});
	auto Pret = XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix);
	for (int32_t i = 0; i < iNumBond; ++i)
	{
		//if (!m_Nodes[desc[i].chunkIndices[0]]->pActor || !m_Nodes[desc[i].chunkIndices[1]]->pActor)
		//{
		//	continue;
		//}
		m_Edges[desc[i].chunkIndices[0]][desc[i].chunkIndices[1]].fHealth = m_fForceThreshold;
		m_Edges[desc[i].chunkIndices[1]][desc[i].chunkIndices[0]].fHealth = m_fForceThreshold;
		XMStoreFloat3(&m_Edges[desc[i].chunkIndices[0]][desc[i].chunkIndices[1]].vCenter, 
			XMVector3TransformCoord({desc[i].bond.centroid[0],desc[i].bond.centroid[1],desc[i].bond.centroid[2], 0.f}, Pret));
		m_Edges[desc[i].chunkIndices[1]][desc[i].chunkIndices[0]].vCenter = m_Edges[desc[i].chunkIndices[0]][desc[i].chunkIndices[1]].vCenter;


		//PxFixedJoint* pJoint = PxFixedJointCreate(*GAMEINSTANCE->Get_Physics(), m_Nodes[desc[i].chunkIndices[1]]->pActor, PxTransform(PxIdentity),
		//	m_Nodes[desc[i].chunkIndices[0]]->pActor, PxTransform(PxIdentity));
		//
		//pJoint->setBreakForce(m_fForceThreshold, m_fTorqueThreshold);
		
		//pJoint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
		//pJoint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
		//pJoint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);
		//pJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
		//pJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
		//pJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);
	}
	m_Actors.push_back({});
	XMStoreFloat4x4(&m_Actors[0].m_Transform, m_pTransformCom->Get_WorldMatrix_XMMat());
	for (_uint i = 1; i <= m_iNumCell; ++i)
		m_Actors[0].Nodes.push_back(i);
	m_Actors[0].pRigidBody = GAMEINSTANCE->Get_Physics()->createRigidDynamic(PxTransform{ XMMatrixToPxMat(XMMatrixNormalize(m_pTransformCom->Get_WorldMatrix_XMMat())) });
	for (auto& shape : m_Shapes)
	{
		if(shape)
		m_Actors[0].pRigidBody->attachShape(*shape);
	}
	m_Actors[0].pRigidBody->userData = this;
	PxRigidBodyExt::updateMassAndInertia(*m_Actors[0].pRigidBody, 50.f);
	GAMEINSTANCE->Get_Scene()->addActor(*m_Actors[0].pRigidBody);
	m_OwnerActor = vector<_uint>(m_iNumCell + 1, 0);
	delete[] isSuporting;
	//NVBLAST_FREE(desc);
	//m_Nodes[0]->pActor = GAMEINSTANCE->Get_Physics()->createRigidDynamic(PxTransform{ PxIdentity });
}

void CBreakable::Build_Hierarchy()
{
	m_Nodes.clear();
	m_Meshes.clear();

	uint32_t numTotalChunks = m_pFractureTool->getChunkCount();
	vector<shared_ptr<CBone>> Bone{};
	_matrix worldMat;

	worldMat = m_pTransformCom->Get_WorldMatrix_XMMat();

	PxVec3 vScale{ XMVector3Length(worldMat.r[0]).m128_f32[0],
	   XMVector3Length(worldMat.r[1]).m128_f32[0],
	   XMVector3Length(worldMat.r[2]).m128_f32[0] };

	for (uint32_t i = 0; i < numTotalChunks; ++i)
	{
		auto& chunkInfo = m_pFractureTool->getChunkInfo(m_pFractureTool->getChunkInfoIndex(i));
		if (chunkInfo.isLeaf)
		{

			shared_ptr<ChunkNode> pNode = make_shared<ChunkNode>();
			pNode->Initialize(chunkInfo);
			auto& parentInfo = m_pFractureTool->getChunkInfo(m_pFractureTool->getChunkInfoIndex(chunkInfo.parentChunkId));
			if (!m_Nodes[parentInfo.chunkId])
			{
				m_Nodes[parentInfo.chunkId] = make_shared<ChunkNode>();
				m_Nodes[parentInfo.chunkId]->Initialize(parentInfo);
			}
			m_Nodes[parentInfo.chunkId]->pChildren.insert(pNode);
			pNode->pParent = m_Nodes[parentInfo.chunkId];
			m_Nodes[chunkInfo.chunkId] = pNode;
			auto pData = Build_Mesh(chunkInfo.chunkId);

			pNode->MeshIdx = m_Meshes.size();
			m_Meshes.push_back(CMesh::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, pData, Bone, XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix)));
			
			m_Meshes.back()->Cooking_ConvexMesh(vScale);

			PxConvexMesh* pTM = m_Meshes.back()->Get_ConvexMesh();
			auto& Center = m_Meshes.back()->ComputeCenter();
			_float3 vCenter{ Center.x,Center.y,Center.z };
	
			if (pTM)
			{
				PxConvexMeshGeometry Geo{ pTM };
				PxMaterial* mat = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.4f, 0.5f);

				//Geo.scale = vScale;
				assert(Geo.isValid());
				PxShape* pShape = GAMEINSTANCE->Get_Physics()->createShape(Geo, *mat);

				pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
				pShape->setSimulationFilterData(PxFilterData{ 1,1,1,1 });
				//m_Shapes.push_back(pShape);
				//pNode->pActor = GAMEINSTANCE->Get_Physics()->createRigidDynamic(PxTransform{ XMMatrixToPxMat(XMMatrixNormalize(worldMat))});
				//pNode->pActor->attachShape(*pShape);
				//
				//pNode->pActor->userData = this;
				//pNode->pActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
				//PxRigidBodyExt::updateMassAndInertia(*pNode->pActor, 50.f);
				m_Shapes.push_back(pShape);
				pShape->acquireReference();
				//GAMEINSTANCE->Get_Scene()->addActor(*pNode->pActor);
			}

			int32_t currId = parentInfo.chunkId;
			while (m_pFractureTool->getChunkInfo(m_pFractureTool->getChunkInfoIndex(currId)).parentChunkId != -1)
			{
				auto& currInfo = m_pFractureTool->getChunkInfo(m_pFractureTool->getChunkInfoIndex(currId));
				auto& nextInfo = m_pFractureTool->getChunkInfo(m_pFractureTool->getChunkInfoIndex(currInfo.parentChunkId));
				auto pCurrNode = m_Nodes[currInfo.chunkId];
				if (!m_Nodes[nextInfo.chunkId])
				{
					m_Nodes[nextInfo.chunkId] = make_shared<ChunkNode>();
					m_Nodes[nextInfo.chunkId]->Initialize(parentInfo);
				}
				m_Nodes[nextInfo.chunkId]->pChildren.insert(pNode);
				pCurrNode->pParent = m_Nodes[nextInfo.chunkId];
				currId = nextInfo.chunkId;
			}

		}
	}

	m_Nodes[0]->isRoot = true;

}

void CBreakable::Insert_Nodes()
{
	//PxTransform TP{XMMatrixToPxMat(XMMatrixNormalize(m_pTransformCom->Get_WorldMatrix_XMMat()))};
	auto pScene = GAMEINSTANCE->Get_Scene();
	//for (auto& pNode : m_Nodes)
	//{
	//	if (pNode.second->pActor)
	//	{
	//		pNode.second->pActor->setGlobalPose(TP);
	//		pScene->addActor(*pNode.second->pActor);
	//	}
	//}
	
	if (m_pDynamicColliderCom)
	{
		pScene->removeActor(*m_pDynamicColliderCom->Get_RigidBody());
	}
}

void CBreakable::CheckIsland()
{
	vector<bool> visited(m_iNumCell + 1, false);
	vector<vector<_uint>> ActorsNodes;
	vector<PxTransform> Transform;
	vector<PxVec3> LinearVel;
	vector<PxVec3> AngVel;
	_uint iActorIdx = 0;
	for (_uint i = 1; i < m_iNumCell + 1; ++i)
	{
		if (visited[i])
			continue;
		visited[i] = true;
		
		
		//for (auto& elem : m_Edges[i])
		//{
		//	if (elem.second.fHealth < 0.f)
		//		continue;
		Transform.push_back(m_Actors[m_OwnerActor[i]].pRigidBody->getGlobalPose());
		if (!Transform.back().isValid())
		{
			throw;
		}
		LinearVel.push_back(m_Actors[m_OwnerActor[i]].pRigidBody->getLinearVelocity());
		AngVel.push_back(m_Actors[m_OwnerActor[i]].pRigidBody->getAngularVelocity());
		//	break;
		//}
		ActorsNodes.push_back({i});

		queue<_uint> Q;
		Q.push(i);
		while (Q.size())
		{
			_uint curr = Q.front(); Q.pop();
			for (auto& elem : m_Edges[curr])
			{
				if (!m_Edges[elem.first].count(curr))
					continue;
				if (visited[elem.first] || m_Edges[curr][elem.first].fHealth < 0.f)
					continue;
				m_Edges[curr][elem.first].iActorIdx = iActorIdx;
				m_Edges[elem.first][curr].iActorIdx = iActorIdx;
				visited[elem.first] = true;
				ActorsNodes.back().push_back(elem.first);
				Q.push(elem.first);
			}
		}
		++iActorIdx;
	}
	if (iActorIdx == m_Actors.size())
		return;

	for (auto& elem : m_Actors)
	{
		elem.pRigidBody->release();
	}
	m_Actors.clear();

	for (_uint i = 0; i < iActorIdx; ++i)
	{
		m_Actors.push_back({});
		m_Actors.back().pRigidBody = GAMEINSTANCE->Get_Physics()->createRigidDynamic(Transform[i]);
		m_Actors.back().pRigidBody->setLinearVelocity(LinearVel[i]);
		m_Actors.back().pRigidBody->setAngularVelocity(AngVel[i]);
		m_Actors.back().pRigidBody->userData = this;
		m_Actors.back().Nodes = (ActorsNodes[i]);
		for (auto idx : m_Actors.back().Nodes)
		{
			if (!m_Nodes.count(idx))
				continue;
			m_OwnerActor[idx] = i;
			m_Actors.back().pRigidBody->attachShape(*m_Shapes[m_Nodes[idx]->MeshIdx]);
		}
		PxRigidBodyExt::updateMassAndInertia(*m_Actors.back().pRigidBody, 50.f);
		GAMEINSTANCE->Get_Scene()->addActor(*m_Actors.back().pRigidBody);
	}

}

void CBreakable::DamageFunction(PxContactPairPoint& pp, _int iActor0Idx, _int iActor1Idx)
{
	_float fMagnitude = pp.impulse.magnitude();
	for (auto& elem0 : m_Edges)
	{
		for (auto& elem1 : elem0)
		{
			if (elem1.second.fHealth < 0.f || (elem1.second.iActorIdx != iActor0Idx && elem1.second.iActorIdx != iActor1Idx))
				continue;
			_vector vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&elem1.second.vCenter), XMLoadFloat4x4(&m_Actors[elem1.second.iActorIdx].m_Transform));
			_float fDist = XMVector3Length(vWorldPos - _vector{ pp.position.x, pp.position.y,pp.position.z }).m128_f32[0];
			_float fPower = fMagnitude / (max(fDist, 0.01f) * max(fDist, 0.01f));

			if (fDist < 2.f && fPower > m_fForceThreshold)
			{
				elem1.second.fHealth -= fPower;
				if (elem1.second.fHealth <= 0.f)
					m_bDirty = true;
			}
		}
	}
}



shared_ptr<MESH_DATA> CBreakable::Build_Mesh(_uint i)
{
	Nv::Blast::Triangle* triangles = nullptr;

	uint32_t numTri = m_pFractureTool->getBaseMesh(i, triangles);
	shared_ptr<MESH_DATA> pMeshData = make_shared<MESH_DATA>();
	pMeshData->iNumVertices = numTri * 3;
	pMeshData->iNumFaces = numTri;

	pMeshData->pVertices = shared_ptr<VTXMESH[]>(new VTXMESH[pMeshData->iNumVertices]);

	pMeshData->pIndices = shared_ptr<FACEINDICES32[]>(new FACEINDICES32[pMeshData->iNumFaces]);
	pMeshData->iMaterialIndex = 0;
	pMeshData->eModelType = MODEL_TYPE::NONANIM;
	pMeshData->iNumBones = 0;
	pMeshData->Bone_Datas.clear();
	pMeshData->szName = "";
	if (triangles)
	{
		for (uint32_t j = 0; j < numTri; ++j)
		{
			memcpy(&pMeshData->pVertices[j * 3].vPosition, &triangles[j].a.p, sizeof(_float3));
			memcpy(&pMeshData->pVertices[j * 3].vNormal, &triangles[j].a.n, sizeof(_float3));
			memcpy(&pMeshData->pVertices[j * 3].vTexcoord, &triangles[j].a.uv[0], sizeof(_float2));

			memcpy(&pMeshData->pVertices[j * 3 + 1].vPosition, &triangles[j].b.p, sizeof(_float3));
			memcpy(&pMeshData->pVertices[j * 3 + 1].vNormal, &triangles[j].b.n, sizeof(_float3));
			memcpy(&pMeshData->pVertices[j * 3 + 1].vTexcoord, &triangles[j].b.uv[0], sizeof(_float2));

			memcpy(&pMeshData->pVertices[j * 3 + 2].vPosition, &triangles[j].c.p, sizeof(_float3));
			memcpy(&pMeshData->pVertices[j * 3 + 2].vNormal, &triangles[j].c.n, sizeof(_float3));
			memcpy(&pMeshData->pVertices[j * 3 + 2].vTexcoord, &triangles[j].c.uv[0], sizeof(_float2));
			if (triangles[j].materialId == m_pFractureTool->getInteriorMaterialId())
			{
				pMeshData->pVertices[j * 3].vTexcoord.x += 1.f;
				pMeshData->pVertices[j * 3 + 1].vTexcoord.x += 1.f;
				pMeshData->pVertices[j * 3 + 2].vTexcoord.x += 1.f;
				pMeshData->pVertices[j * 3].vTexcoord.y += 1.f;
				pMeshData->pVertices[j * 3 + 1].vTexcoord.y += 1.f;
				pMeshData->pVertices[j * 3 + 2].vTexcoord.y += 1.f;
			}
			pMeshData->pIndices[j]._1 = j * 3;
			pMeshData->pIndices[j]._2 = j * 3 + 1;
			pMeshData->pIndices[j]._3 = j * 3 + 2;
			pMeshData->pIndices[j].numIndices = 3;
		}
	}
	return pMeshData;
}




HRESULT CBreakable::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_SphereModel")));

	CPxCollider::PXCOLLIDER_DESC Desc{};
	Desc.pOwner = this;

	Desc.fStaticFriction = 0.4f;
	Desc.fDynamicFriction = 0.5f;
	Desc.fRestitution = 0.5f;
	Desc.strModelComTag = TEXT("Com_SphereModel");
	m_pDynamicColliderCom = static_pointer_cast<CDynamicCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_DynamicCollider"), TEXT("Com_DynamicCollider"), &Desc));
	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CBreakable> CBreakable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBreakable
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBreakable(pDevice, pContext)
		{
		}
	};


	shared_ptr<CBreakable> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBreakable"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CBreakable::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBreakable
	{
		MakeSharedEnabler(const CBreakable& rhs) : CBreakable(rhs)
		{
		}
	};


	shared_ptr<CBreakable> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBreakable"));
		assert(false);
	}

	return pInstance;
}

void CBreakable::Free()
{
	for (auto Shape : m_Shapes)
	{
		if(Shape)
			Shape->release();
	}
}

void CBreakable::ChunkNode::Initialize(const Nv::Blast::ChunkInfo& Info)
{
	isLeaf = Info.isLeaf;
	parentId = Info.parentChunkId;
	chunkId = Info.chunkId;
	auto& BB = Info.getMesh()->getBoundingBox();
	memcpy(&maximum, &BB.maximum, sizeof(_float3));
	memcpy(&minimum, &BB.minimum, sizeof(_float3));
	center.x = (maximum.x + minimum.x) * 0.5f;
	center.y = (maximum.y + minimum.y) * 0.5f;
	center.z = (maximum.z + minimum.z) * 0.5f;

}

_bool CBreakable::ChunkNode::Intersect(const _float3& pos, _float fRadius) const
{
	if (fRadius < 0.f)
		fRadius = -fRadius;
	return
		(maximum.x + fRadius - pos.x >= 0.f &&
			maximum.y + fRadius - pos.y >= 0.f &&
			maximum.z + fRadius - pos.z >= 0.f &&
			minimum.x - fRadius - pos.x <= 0.f &&
			minimum.y - fRadius - pos.y <= 0.f &&
			minimum.z - fRadius - pos.z <= 0.f);
}

vector<_uint> CBreakable::ChunkNode::Get_Shapes() const
{
	vector<_uint> rt;
	if (isLeaf)
	{
		rt.push_back(MeshIdx);
		return rt;
	}
	for (auto& Children : pChildren)
	{
		auto child = Children->Get_Shapes();
		rt.insert(rt.end(), child.begin(), child.end());
	}
	return rt;
}
