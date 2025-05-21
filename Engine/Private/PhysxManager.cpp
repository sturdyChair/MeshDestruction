#include "PhysxManager.h"
#include "GameObject.h"
#include <iostream>

//PxFilterFlags filterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
//	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
//	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
//	pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_CONTACT_POINTS
//		| PxPairFlag::ePRE_SOLVER_VELOCITY | PxPairFlag::ePOST_SOLVER_VELOCITY;
//	return PxFilterFlag::eDEFAULT;
//}

CPhysxManager::CPhysxManager()
{
}

CPhysxManager::~CPhysxManager()
{
	Free();
}

HRESULT CPhysxManager::Initialize()
{
	// Foundation을 만들어 줍니다.
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	// PhysX를 만들어 줍니다.
//#ifdef _DEBUG
	mTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10000);
	if (mTransport == NULL)
		return E_FAIL;
	physx::PxPvdInstrumentationFlags mPvdFlags = physx::PxPvdInstrumentationFlag::eALL;
	mPvd = physx::PxCreatePvd(*m_pFoundation);
	mPvd->connect(*mTransport, mPvdFlags);
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, mPvd);
//#else
	//m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);

//#endif
	//m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, PxCudaContextManagerDesc());
	//if (!m_pCudaContextManager)
	//	assert(false);

	// Scene을 Set 해줍니다.
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // 중력 세팅
	// Dispatcher를 Set 만들어 줍니다.
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = FilterShaderExample;
	sceneDesc.flags.raise(PxSceneFlag::eENABLE_GPU_DYNAMICS);
	sceneDesc.flags.raise(PxSceneFlag::eENABLE_CCD);
	sceneDesc.simulationEventCallback = new MyContactReportCallback{};
	//sceneDesc.filterShader = filterShader;

	// 위에서 설정한대로 Scene을 만들어 줍니다.
	m_pScene = m_pPhysics->createScene(sceneDesc);
	
	// Material을 만들어 줍니다.
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f); // 충돌체 마찰력,Dynamic마찰력, 탄성력

	m_pCCTManager = PxCreateControllerManager(*m_pScene);
	m_pCCTManager->setTessellation(true, 5.f);
//#ifdef _DEBUG
	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	if (pvdClient)
	{
	}
	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);

//#endif

	return S_OK;
}

HRESULT CPhysxManager::Simulate(_float fTimeDelta)
{
	if (!m_bSimulate)
	{
		return S_OK;
	}
	_float fSimInterval = 0.0166f;
	m_fTimer += fTimeDelta;
	if (m_fTimer >= fSimInterval)
	{
		int iterLimit = 4;
		while (m_fTimer >= fSimInterval && iterLimit > 0)
		{
			m_fTimer -= fSimInterval;
			--iterLimit;
			if (!m_pScene->simulate(fSimInterval))
			{
				return E_FAIL;
			}
			if (!m_pScene->fetchResults(true))
			{
				return E_FAIL;
			}
			m_pScene->fetchResultsParticleSystem();
		}

	}

	return S_OK;
}

HRESULT CPhysxManager::Add_Actor(PxActor* pActor, _fmatrix Transform)
{
	if (m_pScene->addActor(*pActor))
	{
		if (pActor->is<PxRigidActor>())
		{
			//_float4x4 fTrans;
			//XMStoreFloat4x4(&fTrans, Transform);
			//_float4 vQ;
			//XMStoreFloat4(&vQ, XMQuaternionRotationMatrix(Transform));
			//_matrix Transposed = XMMatrixTranspose(Transform);
			PxMat44 mat = XMMatrixToPxMat(Transform);


			/*PxQuat q;
			q.x = vQ.x;
			q.y = vQ.y;
			q.z = vQ.z;
			q.w = vQ.w;
			_float3 vPos;
			XMStoreFloat3(&vPos, Transform.r[3]);
			PxVec3 p;
			p.x = vPos.x;
			p.y = vPos.y;
			p.z = vPos.z;*/
		
			//((PxRigidActor*)pActor)->setGlobalPose(PxTransform(p, q));
			((PxRigidActor*)pActor)->setGlobalPose(PxTransform(mat));
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CPhysxManager::Remove_Actor(PxActor* pActor)
{
	m_pScene->removeActor(*pActor);
	return S_OK;
}

HRESULT CPhysxManager::Add_Shape(const wstring& strTag, const vector<_float3>& vecPosition, const vector<_uint>& vecIndicies)
{
	if (m_Shapes.count(strTag) || !vecPosition.size())
	{
		return E_FAIL;
	}

	PxTriangleMeshDesc TMD;

	TMD.points.count = static_cast<PxU32>(vecPosition.size());
	TMD.points.stride = sizeof(_float3);
	TMD.points.data = vecPosition.data();

	TMD.triangles.count = static_cast<PxU32>(vecIndicies.size() / 3);
	TMD.triangles.stride = 3 * sizeof(_uint);
	TMD.triangles.data = vecIndicies.data();
	PxTolerancesScale scale;
	scale.length = 1.0f;  // 기본 단위 길이
	scale.speed = 9.81f;  //
	PxCookingParams cookingParams(scale);
	cookingParams.suppressTriangleMeshRemapTable = true;
	cookingParams.buildTriangleAdjacencies = true;
	cookingParams.buildGPUData = false;
	cookingParams.convexMeshCookingType = PxConvexMeshCookingType::eQUICKHULL;
	cookingParams.areaTestEpsilon = 0.01f;
	cookingParams.planeTolerance = 0.001f;

	PxDefaultMemoryOutputStream writeBuffer;

	if (!PxCookTriangleMesh(cookingParams, TMD, writeBuffer))
		return E_FAIL;
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = m_pPhysics->createTriangleMesh(readBuffer);
	PxShape* pShape = m_pPhysics->createShape(PxTriangleMeshGeometry(triangleMesh), *m_pMaterial);
	m_Shapes[strTag] = pShape;
	triangleMesh->release();
	return S_OK;
}

PxShape* CPhysxManager::Get_Shape(const wstring& strTag)
{
	if (m_Shapes.count(strTag))
	{
		return m_Shapes[strTag];
	}
	return nullptr;
}

bool CPhysxManager::Sweep(PxTransform& StartTransform, PxVec3& vDirection, _float fDistance, _float fRadius, PxSweepBuffer& HitOut)
{
	PxReal sweepDistance = fDistance;

	PxSphereGeometry sphereGeometry(fRadius);

	bool status = m_pScene->sweep(sphereGeometry, StartTransform, vDirection, sweepDistance, HitOut);

	return status;
}

PxMat44 CPhysxManager::XMMatrixToPxMat(_fmatrix matrix)
{
	PxMat44 mat;
	mat.column0.x = matrix.r[0].m128_f32[0];
	mat.column0.y = matrix.r[0].m128_f32[1];
	mat.column0.z = matrix.r[0].m128_f32[2];
	mat.column0.w = matrix.r[0].m128_f32[3];
	mat.column1.x = matrix.r[1].m128_f32[0];
	mat.column1.y = matrix.r[1].m128_f32[1];
	mat.column1.z = matrix.r[1].m128_f32[2];
	mat.column1.w = matrix.r[1].m128_f32[3];
	mat.column2.x = matrix.r[2].m128_f32[0];
	mat.column2.y = matrix.r[2].m128_f32[1];
	mat.column2.z = matrix.r[2].m128_f32[2];
	mat.column2.w = matrix.r[2].m128_f32[3];
	mat.column3.x = matrix.r[3].m128_f32[0];
	mat.column3.y = matrix.r[3].m128_f32[1];
	mat.column3.z = matrix.r[3].m128_f32[2];
	mat.column3.w = matrix.r[3].m128_f32[3];
	return mat;
}


shared_ptr<CPhysxManager> CPhysxManager::Create()
{
	MAKE_SHARED_ENABLER(CPhysxManager)

	shared_ptr<CPhysxManager> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		assert(false);
		//Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysxManager::Free()
{
	for (auto& pShape : m_Shapes)
	{
		pShape.second->release();
	}
	m_pDispatcher->release();
	//m_pCudaContextManager->release();
	m_pMaterial->release();
	m_pCCTManager->release();
	m_pScene->release();

	m_pPhysics->release();
#ifdef _DEBUG
	mPvd->release();
	mTransport->release();
#endif // 
	m_pFoundation->release();

}

void MyContactReportCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	if (pairHeader.actors[0]->userData && pairHeader.actors[1]->userData)
	{
		CGameObject* pGO0 = (CGameObject*)pairHeader.actors[0]->userData;
		CGameObject* pGO1 = (CGameObject*)pairHeader.actors[1]->userData;
		pGO0->OnPxContact(pairHeader, pairs, nbPairs, pGO1);
		pGO1->OnPxContact(pairHeader, pairs, nbPairs, pGO0);
	}
}

void MyContactReportCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void MyContactReportCallback::onWake(PxActor** actors, PxU32 count)
{
}

void MyContactReportCallback::onSleep(PxActor** actors, PxU32 count)
{
}

void MyContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
}

void MyContactReportCallback::onAdvance(const PxGpuMirroredPointer<PxGpuParticleSystem>& gpuParticleSystem, CUstream stream)
{
}
