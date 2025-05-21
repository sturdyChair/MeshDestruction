#include "CSG_Tester.h"
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

wstring CCSG_Tester::ObjID = TEXT("CCSG_Tester");

CCSG_Tester::CCSG_Tester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCSG_Tester::CCSG_Tester(const CCSG_Tester& rhs)
	: CGameObject(rhs)
{
}

CCSG_Tester::~CCSG_Tester()
{
	Free();
}

HRESULT CCSG_Tester::Initialize_Prototype()
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

	IMGUIMANAGER->Add_Mesh(TEXT("Prototype_Component_Model_Torus"));
	IMGUIMANAGER->Add_Mesh(TEXT("Prototype_Component_Model_Cube_Cut"));
	IMGUIMANAGER->Add_Mesh(TEXT("Prototype_Component_Model_Cube"));
	IMGUIMANAGER->Add_Mesh(TEXT("Prototype_Component_Model_Sphere"));
	IMGUIMANAGER->Add_Mesh(TEXT("Prototype_Component_Model_Monkey"));
	return S_OK;
}

HRESULT CCSG_Tester::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;
	QueryPerformanceCounter(&m_CurTime);		
	QueryPerformanceCounter(&m_OldTime);		
	QueryPerformanceCounter(&m_OriginTime);		

	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

HRESULT CCSG_Tester::Initialize_InstantFracture(shared_ptr<CModel> pModel, const FractureOptions& tOptions, _float fExplodeIntensity, shared_ptr<CTransform> pTransform)
{
	m_pTransformCom = pTransform;
	m_pTransformCom->Set_Owner(shared_from_this());
	m_pSphereModelCom = pModel;
	m_pSphereModelCom->Set_Owner(shared_from_this());
	m_Components.emplace(TEXT("Com_SphereModel"), m_pSphereModelCom);
	m_bInstantFracture = true;
	if (FAILED(Ready_Components()))
	{
		MSG_BOX(TEXT("CCSG_Tester::Initialize_InstantFracture : Failed to Initialize_Components"));
		return E_FAIL;
	}

	m_fExplodeIntensity = fExplodeIntensity;

	XMStoreFloat4x4(&m_pSphereModelCom->Get_ModelData()->TransformMatrix, XMMatrixNormalize(XMLoadFloat4x4(&m_pSphereModelCom->Get_ModelData()->TransformMatrix)) );
	//m_pSphereModelCom->Get_ModelData()->TransformMatrix = Identity;

	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_OldTime);
	QueryPerformanceCounter(&m_OriginTime);

	QueryPerformanceFrequency(&m_CpuTick);

	Dispatch_Fracture(tOptions);

	return S_OK;
}

void CCSG_Tester::PriorityTick(_float fTimeDelta)
{
	if (m_bWorking)
	{
		m_fDebugTimer += fTimeDelta;
		//cout << m_fDebugTimer << '\n';
	}
	if (m_fDebugTimer > 0.001f && !m_bWorking)
	{
		m_fDebugTimer = 0.f;
		if (m_bInstantFracture)
		{
			PxSweepHit hit{};
			_float3 vDir = { 0.f, 1.f, 0.f };
			Simulate(hit, vDir);
		}
	}
}

void CCSG_Tester::Tick(_float fTimeDelta)
{
	if (GAMEINSTANCE->Get_KeyPressing(DIK_LCONTROL) && GAMEINSTANCE->Get_KeyDown(DIK_S))
	{
		Bake_Binary();
	}

	if (!m_bHit)
	{
		m_pStaticCollider->Update(fTimeDelta);
		return;
	}
	_uint idx = 0;
	_bool bAllSleep = true;
	_matrix InvWorldMat;
	//if (m_bInstantFracture)
	//	InvWorldMat = XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix);
	//else
		InvWorldMat = XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
		_float3 tempScale = m_pTransformCom->Get_Scale();
	_vector Scale = XMLoadFloat3(&tempScale);
	InvWorldMat = XMMatrixNormalize(InvWorldMat);
	InvWorldMat = XMMatrixInverse(nullptr, InvWorldMat);
	vector<_bool> visited(m_Actors.size(), false);
	for (auto pActor : m_Actors)
	{

		if (m_fSimMaxTick - m_fSimTotalTick > 0.33f && m_bCrusterFracture && m_ParentCluster[idx] == idx && pActor && !m_Detached[idx])
		{
			auto [linearP, angularP] = m_PrevVelocity[idx];
			PxVec3 linearC = pActor->getLinearVelocity();
			PxVec3 angularC = pActor->getAngularVelocity();
			PxVec3 Force = ComputeExternalForce(pActor->getMass(), linearP, linearC, fTimeDelta);
			PxVec3 Torque = ComputeExternalTorque(LocalinertiaTensorToWorld(pActor), angularP, angularC, fTimeDelta);

			_float fTotalOuterForce = Force.magnitude() + Torque.magnitude();
			if (fTotalOuterForce  >= m_fFractureThreshold)
			{
				for (auto child : m_ChildCluster[idx])
				{
					m_Detached[child] = true;
					PxShape* pShape;
					m_Actors[idx]->getShapes(&pShape, 1, 1);
					if(pShape)
					{
						pShape->acquireReference();
						m_Actors[idx]->detachShape(*pShape);
						m_Actors[child] = GAMEINSTANCE->Get_Physics()->createRigidDynamic(m_Actors[idx]->getGlobalPose());
						m_Actors[child]->setAngularVelocity(angularC);
						m_Actors[child]->setLinearVelocity(linearC);
						m_Actors[child]->attachShape(*pShape);
						PxRigidBodyExt::updateMassAndInertia(*m_Actors[child], 0.5f);
						GAMEINSTANCE->Get_Scene()->addActor(*m_Actors[child]);
						
						pShape->release();
					}
				}
				PxRigidBodyExt::updateMassAndInertia(*m_Actors[idx], 0.5f);
				
				m_Detached[idx] = true;
			}

			m_PrevVelocity[idx] = {linearC, angularC};
		}
		++idx;
		if (pActor)
		{
			PxTransform Pose = pActor->getGlobalPose();
			if (!pActor->isSleeping())
				bAllSleep = false;
			PxMat44 mat(Pose);
			_matrix world = PxMatToXMMatrix(mat) * InvWorldMat;
			_vector center = XMLoadFloat3(&m_Centers[idx - 1]);
		
			//world.r[3] -= center;
			if(m_bInstantFracture)
			{
				world.r[3] /= Scale;
			}
			world.r[3].m128_f32[3] = 1.f;
			m_pModelCom->Set_BoneMatrix(idx, world);
		}
	}
	m_pModelCom->Reset_Bones();
	if(!m_bInstantFracture)
		m_fSimTotalTick -= fTimeDelta;
	m_fSimTick += fTimeDelta;

	if ((m_fSimTick >= 0.25f || bAllSleep) && m_pCurrAnimData)
	{
		m_fSimTick -= 0.25f;
		idx = 0;
		for (auto pActor : m_Actors)
		{
			if (pActor)
			{
				
				PxTransform Pose = pActor->getGlobalPose();
				PxMat44 mat(Pose);
				_matrix world = PxMatToXMMatrix(mat) * InvWorldMat;
				_vector vScale, vRot, vTrans;
				XMMatrixDecompose(&vScale, &vRot, &vTrans, world);
				
				KEYFRAME kf{};
				kf.fTime = m_fSimMaxTick - m_fSimTotalTick;
				kf.fTrackPosition = m_fSimMaxTick - m_fSimTotalTick;
				XMStoreFloat4(&kf.vRotation, vRot);
				XMStoreFloat3(&kf.vScale, vScale);
				XMStoreFloat3(&kf.vTranslation, vTrans);

				m_pCurrAnimData->Channel_Datas[idx]->tKeyFrames.push_back(kf);
			}
			++idx;
		}
	}
	if((bAllSleep && !m_bInstantFracture) || m_fSimTotalTick <= 0.f || (m_bInstantFracture && GAMEINSTANCE->Get_KeyDown(DIK_RETURN)))
	{
		End_Sim();
	}
}

void CCSG_Tester::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CCSG_Tester::Render()
{
	if (m_bHit && !m_bWorking)
		return Render_Dynamic();
	else
		return Render_Static();

	return S_OK;
}

void CCSG_Tester::Simulate(const PxSweepHit& Hit, const _float3& vDir)
{
	if (m_bHit || !m_pModelCom || m_bWorking)
		return;
	m_bHit = true;
	auto pScene = GAMEINSTANCE->Get_Scene();
	auto worldMat = XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
	auto InvWorldMat = XMMatrixInverse(nullptr, worldMat);
	PxVec3 vScale{ XMVector3Length(worldMat.r[0]).m128_f32[0],
		   XMVector3Length(worldMat.r[1]).m128_f32[0],
		   XMVector3Length(worldMat.r[2]).m128_f32[0] };
	auto TP = PxTransform(XMMatrixToPxMat(worldMat));
	_uint idx = 0;
	if(!m_bInstantFracture)
	{
		m_pCurrAnimData = make_shared<ANIMATION_DATA>();
		m_pCurrAnimData->szName = string("simulation_") + to_string(m_iSimCount);
		m_pCurrAnimData->fTickPerSecond = 1.f;
	}
	
	for (auto pActor : m_Actors)
	{
		if (m_bCrusterFracture)
		{
			m_Detached[idx] = false;
			m_PrevVelocity[idx] = { {0.f,0.f,0.f},{0.f,0.f,0.f} };
		}
		++idx;
		if (pActor)
		{
			PxRigidBodyExt::updateMassAndInertia(*pActor, 0.5f);
			pScene->addActor(*pActor);
			pActor->setGlobalPose(TP);
			pActor->addForce(PxVec3(vDir.x, vDir.y, vDir.z) * 0.001f, PxForceMode::eIMPULSE);
			if (m_bInstantFracture)
			{
				_float fRandX = CMath_Manager::Random_Float(-m_fExplodeIntensity, m_fExplodeIntensity);
				_float fRandY = CMath_Manager::Random_Float(-m_fExplodeIntensity, m_fExplodeIntensity);
				_float fRandZ = CMath_Manager::Random_Float(-m_fExplodeIntensity, m_fExplodeIntensity);
				pActor->addForce(PxVec3(fRandX, fRandY, fRandZ), PxForceMode::eIMPULSE);
			}
		}
		if (!m_bInstantFracture)
		{
			auto pCD = make_shared<CHANNEL_DATA>();
			pCD->szNodeName = m_pModelCom->Get_BoneName(idx);
			KEYFRAME kf{};
			kf.fTime = 0.f;
			kf.fTrackPosition = 0.f;
			_vector vRot, vScale, vTrans;
			XMMatrixDecompose(&vScale, &vRot, &vTrans, XMMatrixIdentity());
			XMStoreFloat4(&kf.vRotation, vRot);
			XMStoreFloat3(&kf.vScale, vScale);
			XMStoreFloat3(&kf.vTranslation, vTrans);
			pCD->tKeyFrames.push_back(kf);
			m_pCurrAnimData->Channel_Datas.push_back(pCD);
		}
	}
	if (!m_bInstantFracture)
	{
		m_pCurrAnimData->iNumChannels = m_pCurrAnimData->Channel_Datas.size();
	}
	m_pStaticCollider->Remove_Actor();
	m_fSimTotalTick = m_fSimMaxTick;
	HIT myHit{};
	_vector vHitPoint = XMVector4Transform(XMVectorSet(Hit.position.x, Hit.position.y, Hit.position.z, 1.f), InvWorldMat);
	_vector vNormal = XMVector3Transform(XMVectorSet(Hit.normal.x, Hit.normal.y, Hit.normal.z, 0.f), InvWorldMat);
	vNormal = XMVector3Normalize(vNormal);
	_vector vDirection = XMLoadFloat3(&vDir);
	vDirection = XMVector3Normalize(vDirection);
	XMStoreFloat4(&myHit.vHitPoint, vHitPoint);
	XMStoreFloat3(&myHit.vNormal, vNormal);
	XMStoreFloat3(&myHit.vDirection, vDirection);
	m_vecHits.push_back(myHit);
	++m_iSimCount;
}

void CCSG_Tester::End_Sim()
{
	if(m_pStaticCollider)
		m_pStaticCollider->Add_Actor();

	m_bHit = false;
	auto pScene = GAMEINSTANCE->Get_Scene();
	for (auto pActor : m_Actors)
	{
		if (pActor)
		{
			pScene->removeActor(*pActor);
		}

	}
	if (m_bCrusterFracture)
	{
		_uint iSize = m_Actors.size();
		for (_uint i = 0; i < iSize; ++i)
		{
			if (m_Actors[i] && m_Detached[i] && m_ParentCluster[i] != i)
			{
				if(m_Actors[i]->getNbShapes())
				{
					PxShape* pShape;
					m_Actors[i]->getShapes(&pShape, 1);
					pShape->acquireReference();
					m_Actors[i]->detachShape(*pShape);
					m_Actors[i]->release();
					m_Actors[i] = m_Actors[m_ParentCluster[i]];
					m_Actors[i]->attachShape(*pShape);
					pShape->release();
				}
				m_Detached[i] = false;
			}
		}

	}
	if(m_pCurrAnimData)
	{
		for (auto& cd : m_pCurrAnimData->Channel_Datas)
		{
			cd->iNumKeyframes = cd->tKeyFrames.size();
			cd->iNumPositionKeys = cd->iNumKeyframes;
			cd->iNumRotationKeys = cd->iNumKeyframes;
			cd->iNumScalingKeys = cd->iNumKeyframes;
		}
		m_pCurrAnimData->fDuration = m_fSimMaxTick - m_fSimTotalTick;
		m_AnimationDatas.push_back(m_pCurrAnimData);
		m_pModelCom->Get_ModelData()->Animation_Datas.push_back(m_pCurrAnimData);
	}
}

void CCSG_Tester::Bake_Binary()
{
	auto pMD = m_pModelCom->Get_ModelData();
	pMD->iNumAnimations = pMD->Animation_Datas.size();
	pMD->iNumMeshs = pMD->Mesh_Datas.size();
	pMD->iNumMaterials = pMD->Material_Datas.size();
	pMD->eModelType = MODEL_TYPE::ANIM;
	m_pModelCom->Get_ModelData()->Bake_Binary();

	Save_HitInfo();

}

void CCSG_Tester::Apply_Inner_Texture(const string& szFilename, shared_ptr<CTexture> pTexture, aiTextureType eType)
{
	if (!m_pModelCom)
		return;
	m_pModelCom->Get_ModelData()->Material_Datas[1]->szTextureName[eType] = szFilename;
	m_pModelCom->Chander_Material(1, pTexture, eType);
}

void CCSG_Tester::Dispatch_Fracture(const FractureOptions& tOptions)
{
	if (m_bWorking)
		return;
	if (m_Worker.joinable())
		m_Worker.join();
	m_fDebugTimer = 0.f;
	m_Worker = thread(&CCSG_Tester::Fracture, this, tOptions, ref(MDS), ref(CellNeighbors));
	m_bWorking = true;
	m_Worker.detach();
}

void CCSG_Tester::Fracture(const FractureOptions& tOptions, vector<shared_ptr<MODEL_DATA>>& MDS, vector<vector<int>>& CellNeighbors)
{
	if (m_pModelCom)
		End_Sim();
	m_Actors.clear();
	m_tFracOp = tOptions;
	PerformanceCounter();
	if(m_bInstantFracture)
		MDS = m_pSphereModelCom->Blast_Voronoi(tOptions.fractureCount, tOptions.vScale, CellNeighbors, tOptions.vCenter, tOptions.WebNumAngle, tOptions.WebNumLayer, tOptions.mode);
	else
		MDS = m_pSphereModelCom->Do_Voronoi(tOptions.fractureCount, tOptions.vScale, CellNeighbors, tOptions.vCenter,tOptions.WebNumAngle,tOptions.WebNumLayer,tOptions.mode);
	PerformanceCounter();
	cout << "Fracture : " << m_fTimeDelta << '\n';
	Fracture_Internal(tOptions, MDS, CellNeighbors);
	m_bWorking = false;
}

void CCSG_Tester::Fracture_Internal(const FractureOptions& tOptions, vector<shared_ptr<MODEL_DATA>>& MDS, vector<vector<int>>& CellNeighbors)
{
	auto pSphereModelData = m_pSphereModelCom->Get_ModelData();
	vector<int> clusterRoot(CellNeighbors.size(), -1);
	m_bCrusterFracture = tOptions.bCrusterFracture;
	if (m_bCrusterFracture)
	{
		m_Detached.resize(CellNeighbors.size());
		m_ParentCluster = vector<_uint>(CellNeighbors.size());
		m_PrevVelocity.resize(CellNeighbors.size());
		m_ChildCluster.clear();
	}

	for (_uint i = 0; i < CellNeighbors.size(); ++i)
	{
		if (clusterRoot[i] != -1)
			continue;
		clusterRoot[i] = i;
		if (m_bCrusterFracture)
			m_ParentCluster[i] = i;
		_uint iSize = 1;
		for (auto iter = CellNeighbors[i].begin(); iter != CellNeighbors[i].end(); ++iter)
		{
			if (*iter >= 0)
			{
				if (iSize >= tOptions.ClusterSize)
					break;
				if (clusterRoot[*iter] != -1)
					continue;

				clusterRoot[*iter] = i;
				if (m_bCrusterFracture)
				{
					m_ParentCluster[*iter] = i;
					m_ChildCluster[i].push_back(*iter);
				}
				++iSize;
			}
		}
	}
	PerformanceCounter();

	m_pModelCom = CModel::Create(m_pDevice, m_pContext, MDS[0], XMLoadFloat4x4(&pSphereModelData->TransformMatrix));
	PerformanceCounter();
	cout << "Model : " << m_fTimeDelta << '\n';
	auto pPhysics = GAMEINSTANCE->Get_Physics();
	auto pScene = GAMEINSTANCE->Get_Scene();
	_matrix worldMat;
	if(!m_bInstantFracture)
		worldMat = XMLoadFloat4x4(&m_pModelCom->Get_ModelData()->TransformMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
	else
		worldMat = m_pTransformCom->Get_WorldMatrix_XMMat();
	PxVec3 vScale{ XMVector3Length(worldMat.r[0]).m128_f32[0],
	   XMVector3Length(worldMat.r[1]).m128_f32[0],
	   XMVector3Length(worldMat.r[2]).m128_f32[0] };
	PxVec3 pos = { worldMat.r[3].m128_f32[0],worldMat.r[3].m128_f32[1],worldMat.r[3].m128_f32[2] };
	auto TP = PxTransform(XMMatrixToPxMat(XMMatrixNormalize(worldMat)));
	if (!TP.isValid())
		throw;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		auto pMesh = m_pModelCom->Get_Mesh(i);

		PxRigidDynamic* pActor;
		if (clusterRoot[i] == i || !m_Actors[clusterRoot[i]])
		{
			pActor = pPhysics->createRigidDynamic(TP);
			pActor->userData = this;
		}
		else
		{
			pActor = m_Actors[clusterRoot[i]];
		}
		_float fSF = 0.4f, fDF = 0.5f, fRest = 0.5f;

		PxConvexMesh* pTM = pMesh->Get_ConvexMesh();
		auto& Center = pMesh->ComputeCenter();
		_float3 vCenter{ Center.x,Center.y,Center.z };
		m_Centers.push_back(vCenter);
		if (!pTM)
		{
			pMesh->Cooking_ConvexMesh();
			pTM = pMesh->Get_ConvexMesh();
		}
		if (pTM)
		{
			PxConvexMeshGeometry Geo{ pTM };
			PxMaterial* mat = pPhysics->createMaterial(fSF, fDF, fRest);

			Geo.scale = vScale;
			assert(Geo.isValid());
			PxShape* pShape = pPhysics->createShape(Geo, *mat);
			pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			pShape->setSimulationFilterData(PxFilterData{ 1,1,1,1 });
			_bool b = pActor->attachShape(*pShape);

			if (!b) throw;
			pActor->setGlobalPose(PxTransform(pos));
			
		}
		else
		{
			if (clusterRoot[i] == i)
				pActor = nullptr;
		}

		m_Actors.push_back(pActor);

	}
	for (auto& elem : m_Actors)
	{
		if (elem)
		{
			PxRigidBodyExt::updateMassAndInertia(*elem, 1.f);
		}
	}

	m_pModelCom->Reset_Bones();
	PerformanceCounter();
	cout << "Px : " << m_fTimeDelta << '\n';
}

void CCSG_Tester::Save_HitInfo()
{
	auto pMD = m_pModelCom->Get_ModelData();

	string          szBinFilePath;
	char			szDir[MAX_PATH] = "";
	char			szFileName[MAX_PATH] = "";

	_splitpath_s(pMD->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	szBinFilePath = szDir;
	szBinFilePath += szFileName;
	szBinFilePath += ".hit";

	ofstream os(szBinFilePath, ios::binary);
	write_typed_data(os, m_vecHits.size());
	_uint idx = 0;
	for (auto& hit : m_vecHits)
	{
		write_typed_data(os, m_AnimationDatas[idx]->szName.size());
		os.write(&m_AnimationDatas[idx]->szName[0], m_AnimationDatas[idx]->szName.size());
		write_typed_data(os, hit.vHitPoint);
		write_typed_data(os, hit.vDirection);
		write_typed_data(os, hit.vNormal);
		++idx;
	}
}

HRESULT CCSG_Tester::Render_Static()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pSphereModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pSphereModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
		}
		if (m_bWorking)
		{
			_float fColor = m_fDebugTimer * 5.f;
			_float4 vColor = { fColor, fColor ,fColor * 0.1f ,fColor };
			m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
			_float4x4 World;
			XMStoreFloat4x4(&World, m_pTransformCom->Get_WorldMatrix_XMMat()
			* XMMatrixTranslation(CMath_Manager::Random_Float(-0.3f, 0.3f), CMath_Manager::Random_Float(-0.3f, 0.3f), CMath_Manager::Random_Float(-0.3f, 0.3f)));
			m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World);
		
			m_pShaderCom->Begin((_uint)MESH_PASS::EmitColor);
		}
		else
		{
			m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}
		m_pSphereModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CCSG_Tester::Render_Dynamic()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pAnimShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pAnimShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pAnimShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pAnimShaderCom, "g_BoneMatrix", 0u)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Mat_MatIdx(m_pAnimShaderCom, "g_Normal_Texture_In", (_uint)1, aiTextureType_NORMALS)))
	{
		CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pAnimShaderCom, "g_Normal_Texture_In");
	}
	m_pModelCom->Bind_Mat_MatIdx(m_pAnimShaderCom, "g_Texture_In", (_uint)1, aiTextureType_DIFFUSE);

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pAnimShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pAnimShaderCom, "g_ATOS_Texture", (_uint)0, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pAnimShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pAnimShaderCom, "g_Normal_Texture", (_uint)0, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pAnimShaderCom, "g_Normal_Texture");
		}
		m_pAnimShaderCom->Begin((_uint)ANIMMESH_PASS::Destroy);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

void CCSG_Tester::PerformanceCounter()
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




HRESULT CCSG_Tester::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pAnimShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_AnimShader")));
	//m_pCubeModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cube"), TEXT("Com_Model")));
	if(!m_bInstantFracture)
		m_pSphereModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_SphereModel")));


	CPxCollider::PXCOLLIDER_DESC Desc{};
	Desc.pOwner = this;

	Desc.fStaticFriction = 0.4f;
	Desc.fDynamicFriction = 0.5f;
	Desc.fRestitution = 0.5f;
	Desc.strModelComTag = L"Com_SphereModel";
	m_pStaticCollider = static_pointer_cast<CStaticCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StaticCollider"), TEXT("Com_StaticCollider"), &Desc));

	if (!m_pShaderCom || !m_pSphereModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CCSG_Tester> CCSG_Tester::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CCSG_Tester
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCSG_Tester(pDevice, pContext)
		{
		}
	};


	shared_ptr<CCSG_Tester> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CCSG_Tester"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CCSG_Tester::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CCSG_Tester
	{
		MakeSharedEnabler(const CCSG_Tester& rhs) : CCSG_Tester(rhs)
		{
		}
	};


	shared_ptr<CCSG_Tester> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CCSG_Tester"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CCSG_Tester> CCSG_Tester::Create_Instant_Fracture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, shared_ptr<CModel> pStaticModel, 
	const FractureOptions& tOptions, _float fExplodeIntensity, shared_ptr<CTransform> pTransform)
{
	struct MakeSharedEnabler :public CCSG_Tester
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCSG_Tester(pDevice, pContext)
		{
		}
	};
	shared_ptr<CCSG_Tester> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_InstantFracture(pStaticModel, tOptions, fExplodeIntensity, pTransform)))
	{
		MSG_BOX(TEXT("Failed to Create : CCSG_Tester"));
		assert(false);
	}

	return pInstance;
}

void CCSG_Tester::Free()
{
}
