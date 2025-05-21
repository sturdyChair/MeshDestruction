#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCharacterController : public CComponent
{
public:
	struct CCT_DESC : public PxCapsuleControllerDesc
	{
		shared_ptr<class CTransform> pTransform = nullptr;
	};

private:
	CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacterController(const CCharacterController& rhs);
public:
	virtual ~CCharacterController();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Set_Transform(shared_ptr<class CTransform>  pTransform);
	PxController* Get_Controller() { return m_pPxController; }
	void Set_Displacement(const _float3& vDisp) { m_vDisplacement = vDisp; }
	void Set_Displacement(_fvector vDisp) { XMStoreFloat3(&m_vDisplacement, vDisp); }
	void Add_Displacement(const _float3& vDisp);
	void Add_Displacement(_fvector vDisp);

	void Move_Forward(_float fDist);
	void Move_Right(_float fDist);
	void Move_Up(_float fDist);
	void Move_Backward(_float fDist);
	void Move_Left(_float fDist);
	void Move_Down(_float fDist);

	void Move_Land_Forward(_float fDist);
	void Move_Land_Right(_float fDist);
	void Move_Land_Left(_float fDist);
	void Move_Land_Backward(_float fDist);

	void Velocity_Calculation(_float fTimeDelta);

	void Set_Position(_fmatrix Transform);
	void Set_FootPosition(_fmatrix Transform);
	void Update(_float fTimeDelta);

	void Disable(bool bFlag = true);

	void Enable_Gravity();
	void Disable_Gravity();
	
	void Enable_Pseudo_Physics();
	void Disable_Pseudo_Physics();

	const _float3& Get_Velocity() { return m_vVelocity; }
	void Set_Velocity(const _float3& vVel) { m_vVelocity = vVel; }
	void Set_Velocity(_fvector vVel) { XMStoreFloat3(&m_vVelocity, vVel); }
	void Add_Velocity(const _float3& vVel) { m_vVelocity.x += vVel.x;  m_vVelocity.y += vVel.y;  m_vVelocity.z += vVel.z;}
	void Add_Velocity(_fvector vVel) { XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vVel); }
	
	void Set_Accel(const _float3& vAcc) { m_vAccel = vAcc; }
	void Set_Accel(_fvector vAcc) { XMStoreFloat3(&m_vAccel, vAcc); }
	void Add_Accel(const _float3& vAcc) { m_vAccel.x += vAcc.x;  m_vAccel.y += vAcc.y;  m_vAccel.z += vAcc.z;}
	void Add_Accel(_fvector vAcc) { XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vAcc); }

	void Set_Impulse(const _float3& vImp) { m_vImpulse = vImp; }
	void Set_Impulse(_fvector vImp) { XMStoreFloat3(&m_vImpulse, vImp); }
	void Add_Impulse(const _float3& vImp) { m_vImpulse.x += vImp.x;  m_vImpulse.y += vImp.y;  m_vImpulse.z += vImp.z; }
	void Add_Impulse(_fvector vImp) { XMStoreFloat3(&m_vImpulse, XMLoadFloat3(&m_vImpulse) + vImp); }

	void Enable_Foot_Pos_Align();
	void Disable_Foot_Pos_Align();

	void Resize(_float fRadius, _float fHeight);
	void Get_Size(_float& fRadius, _float& fHeight);

private:
	PxController* m_pPxController = nullptr;
	shared_ptr<CTransform> m_pTransform = nullptr;
	shared_ptr<class CPhysxManager> m_pPhysxManager;

private:
	_float3	m_vDisplacement;
	_float3 m_vLook;
	_float3 m_vRight;
	_float3 m_vUp;
	_float m_fHeight = 0.f;
	_float m_fRadius = 0.f;

	bool m_bUseVelocity_Calculation = false;
	_float  m_fMass = { 0.f };
	_float  m_fSpeed = { 0.f };
	_float  m_fMaxSpeed = { 0.f };
	_float  m_fFriction = { 0.f };
	_float  m_fGravity = 40.f;
	bool    m_bGravity = true;
	_float3 m_vVelocity;
	_float3 m_vAccel;
	_float3 m_vImpulse;

	bool m_bActivated = true;
	PxExtendedVec3 m_vPrevPos;
	bool m_bFootPosAlign = true;

	static _float s_fGlobalGravity;

public:
	static shared_ptr<CCharacterController> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END
//PxCapsuleControllerDesc desc;
//desc.height = 2.0f;
//desc.radius = 0.5f;
//desc.position = PxExtendedVec3(0.0f, 2.0f, 0.0f);
//desc.material = gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
//gCharacterController = gControllerManager->createController(desc);