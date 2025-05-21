#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)
class MyContactReportCallback : public PxSimulationEventCallback
{
public:
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;

	// PxSimulationEventCallback을(를) 통해 상속됨
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	void onWake(PxActor** actors, PxU32 count) override;
	void onSleep(PxActor** actors, PxU32 count) override;
	void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
	void onAdvance(const PxGpuMirroredPointer<PxGpuParticleSystem>& gpuParticleSystem, CUstream stream) override;
};
PxFilterFlags FilterShaderExample(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_CONTACT_POINTS;

	return PxFilterFlag::eDEFAULT;
}

class CPhysxManager 
{
private:
	CPhysxManager();
public:
	virtual ~CPhysxManager();

public:
	HRESULT Initialize();

	HRESULT Simulate(_float fTimeDelta);

	PxScene* Get_Scene() const { return m_pScene; }
	PxControllerManager* Get_CCT_Manager() const { return m_pCCTManager; }
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	PxFoundation* Get_Foundation() const { return m_pFoundation; }
	HRESULT Add_Actor(PxActor* pActor, _fmatrix Transform = XMMatrixIdentity());
	HRESULT Remove_Actor(PxActor* pActor);
	// Dynamic, Static 정하기, 시작 위치 정하기...등등
	//HRESULT Add_CCT();
	HRESULT Add_Shape(const wstring& strTag, const vector<_float3>& vecPosition, const vector<_uint>& vecIndicies);
	//HRESULT Add_Shape(const wstring& strTag, const vector<PxVec3>& vecPosition, const vector<_uint>& vecIndicies);
	PxShape* Get_Shape(const wstring& strTag);

	bool Sweep(PxTransform& StartTransform, PxVec3& vDirection, _float fDistance, _float fRadius, PxSweepBuffer& HitOut);
	void Simulate_Physx(_bool bSim = true) { m_bSimulate = bSim; }
	//PxCudaContextManager* Get_CudaContextManager() { return m_pCudaContextManager; }

private:

	PxMat44 XMMatrixToPxMat(_fmatrix matrix);


private:
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation* m_pFoundation = NULL;

	PxPhysics* m_pPhysics = NULL;


	PxDefaultCpuDispatcher* m_pDispatcher = NULL;
	
	PxScene* m_pScene = NULL;

	PxMaterial* m_pMaterial = NULL;

	PxControllerManager* m_pCCTManager = nullptr;
	//PxCudaContextManager* m_pCudaContextManager = nullptr;

	map<wstring, PxShape*> m_Shapes;

	_bool m_bSimulate = true;
	_float m_fTimer = 0.f;
//#ifdef _DEBUG
	physx::PxPvdTransport* mTransport = nullptr;
	physx::PxPvd* mPvd = nullptr;
//#endif
public:
	static shared_ptr<CPhysxManager> Create();
	void Free();
};

END