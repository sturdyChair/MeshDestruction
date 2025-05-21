#include "..\Public\DynamicCollider.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

CDynamicCollider::CDynamicCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPxCollider{ pDevice, pContext }
{
}

CDynamicCollider::CDynamicCollider(const CDynamicCollider& rhs)
	: CPxCollider(rhs)
{
}

CDynamicCollider::~CDynamicCollider()
{
	Free();
}

HRESULT CDynamicCollider::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CDynamicCollider::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	_matrix worldMat = m_pTransform->Get_WorldMatrix_XMMat();
	PxVec3 vScale{ XMVector3Length(worldMat.r[0]).m128_f32[0],
				   XMVector3Length(worldMat.r[1]).m128_f32[0],
				   XMVector3Length(worldMat.r[2]).m128_f32[0] };
	m_vScale.x = vScale.x; m_vScale.y = vScale.y; m_vScale.z = vScale.z;
	worldMat = XMMatrixNormalize(worldMat);
	XMStoreFloat4x4(&m_PrevTransform, worldMat);
	m_pRigidDynamic = m_pPhysics->createRigidDynamic(PxTransform(XMMatrixToPxMat(worldMat)));
	
	PXCOLLIDER_DESC* pDesc = reinterpret_cast<PXCOLLIDER_DESC*>(pArg);
	_float fSF = 0.4f, fDF = 0.5f, fRest = 0.5f;
	if (pDesc)
	{
		fSF = pDesc->fStaticFriction;
		fDF = pDesc->fDynamicFriction;
		fRest = pDesc->fRestitution;
		m_pRigidDynamic->userData = pDesc->pOwner;
	}
	PxMaterial* mat = m_pPhysics->createMaterial(fSF, fDF, fRest);

	Attach_Shape(m_pRigidDynamic, mat, vScale);
	PxRigidBodyExt::updateMassAndInertia(*m_pRigidDynamic, 100.f);
	//m_pRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_pCurrentScene->addActor(*m_pRigidDynamic);
	m_pRigidDynamic->setGlobalPose(PxTransform(XMMatrixToPxMat(worldMat)));
	return S_OK;
}

void CDynamicCollider::Update(_float fTimeDelta)
{
	PxTransform Pose = m_pRigidDynamic->getGlobalPose();
	PxMat44 mat(Pose);
	_matrix world = PxMatToXMMatrix(mat);
	m_pTransform->Set_WorldMatrix(world);
	m_pTransform->Set_Scale(m_vScale);
}

_bool CDynamicCollider::Is_Kinematic()
{
	return m_pRigidDynamic->getRigidBodyFlags()& PxRigidBodyFlag::eKINEMATIC;
}

shared_ptr<CDynamicCollider> CDynamicCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CDynamicCollider
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDynamicCollider(pDevice, pContext) {}
	};

	shared_ptr<CDynamicCollider> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDynamicCollider"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CDynamicCollider::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CDynamicCollider
	{
		MakeSharedEnabler(const CDynamicCollider& rhs) : CDynamicCollider(rhs) {}
	};

	shared_ptr<CDynamicCollider> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDynamicCollider"));
		return nullptr;
	}

	return pInstance;
}

void CDynamicCollider::Free()
{
	if (m_pRigidDynamic)
		m_pCurrentScene->removeActor(*m_pRigidDynamic);
}
