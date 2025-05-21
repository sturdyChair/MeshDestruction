#pragma once
#include "Component.h"
#include "PhysxManager.h"


BEGIN(Engine)
class CTransform;
class CVIBuffer;

class ENGINE_DLL CPhysXCollider : public CComponent
{

	enum COLLIDER_TYPE {
		CCT,
		RIGIDBODY,
		COLLIDER_NONE
	};

private:
	CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXCollider(const CPhysXCollider& rhs);
public:
	virtual ~CPhysXCollider();
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	PHYSXCOLLIDERDESC Get_ColliderDesc() const { return m_PhysXColliderDesc; }



public:
	void	Synchronize_Transform(CTransform* pTransform, _fvector In_vOffset = { 0.f, 0.f, 0.f });
	void	Synchronize_Transform_Position(CTransform* pTransform);
	void	Synchronize_Transform_Rotation(CTransform* pTransform);

	void	checkCollisionFlags(PxU32 collisionFlags);
public:
	PxTriangleMesh* CreateTriangleMesh(PxVec3* vertices, PxU32* indices, PxU32 vertexCount, PxU32 triangleCount);
	PxRigidStatic* createStaticMeshActor(PxTriangleMesh* triangleMesh, const char* PhysXTag);
	PxRigidStatic* createStaticShapeActor(const PxGeometry& geometry, const char* PhysXTag);
	PxRigidStatic* createStaticMeshActor(PxTriangleMesh* triangleMesh, PxVec3 Scale, const char* PhysXTag);
	PxRigidDynamic* createDynamicMeshActor(PxTriangleMesh* triangleMesh, const char* PhysXTag);
	PxRigidDynamic* createDynamicShapeActor(const PxGeometry& geometry, const char* PhysXTag);

	PxRigidDynamic* createDynamicActor(const char* PhysXTag);

	PxController* createCharacterController(PxControllerManager* manager, const PxVec3& position);

	PxRigidDynamic* Get_Dynamic_RigidBody() { return m_pRigidDynamic; }
	PxRigidStatic* Get_Static_RigidBody() { return m_pRigidStatic; }
	PxController* Get_Controller() { return m_pController; }

	_bool Get_CollisionDown() { return m_bCollisionDown; }
	_bool Get_CollisionUp() { return m_bCollisionUp; }
	_bool Get_CollisionLeft() { return m_bCollisionLeft; }
	_bool Get_CollisionRight() { return m_bCollisionRight; }
	_bool Get_CollisionSides() { return m_bCollisionSides; }
	_bool Get_Collision() { return m_bCollision; }
	void Add_To_Scene();
	void Remove_To_Scene();

	void Set_LookVector(PxVec3 vLook) { m_pHitReport->SetLookDirection(vLook); }

public:
	CPhysXCollider* CallBack_CollisionEnter;
	CPhysXCollider* CallBack_CollisionStay;
	CPhysXCollider* CallBack_CollisionExit;

private:
	static	_uint		m_iClonedColliderIndex;
	_uint				m_iColliderIndex;

	COLLIDER_TYPE		m_eColliderType = COLLIDER_TYPE::COLLIDER_NONE;

private:
	_vector					m_vMaxVelocity;

private:


private:
	PHYSXCOLLIDERDESC		m_PhysXColliderDesc;

	PxRigidDynamic* m_pRigidDynamic = nullptr;
	PxRigidStatic* m_pRigidStatic = nullptr;
	PxController* m_pController = nullptr;
	CharacterControllerHitReport* m_pHitReport = nullptr;

	vector<PxConvexMesh*>	m_ConvexMeshes;
	vector<PxTriangleMesh*>	m_TriangleMesh;

	CTransform* m_pTransform = nullptr;

	PxPhysics* m_pPhysics = nullptr;
	PxScene* m_pCurrentScene = nullptr;

	_bool					m_bPickState = false;
	_bool					m_bPickable = true;
	_bool					m_bYFixed = false;


	_bool					m_bCollision = false;

	_bool					m_bCollisionDown = false;
	_bool					m_bCollisionUp = false;
	_bool					m_bCollisionLeft = false;
	_bool					m_bCollisionRight = false;
	_bool					m_bCollisionSides = false;
public:
	static shared_ptr<CPhysXCollider> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END