#include "..\Public\StaticCollider.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

CStaticCollider::CStaticCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPxCollider{ pDevice, pContext }
{
}

CStaticCollider::CStaticCollider(const CStaticCollider& rhs)
	: CPxCollider(rhs)
{
}

CStaticCollider::~CStaticCollider()
{
	Free();
}

HRESULT CStaticCollider::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CStaticCollider::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	_matrix worldMat = m_pTransform->Get_WorldMatrix_XMMat();
	PxVec3 vScale{ XMVector3Length(worldMat.r[0]).m128_f32[0],
				   XMVector3Length(worldMat.r[1]).m128_f32[0],
				   XMVector3Length(worldMat.r[2]).m128_f32[0] };
	worldMat = XMMatrixNormalize(worldMat);
	XMStoreFloat4x4(&m_PrevTransform, worldMat);
	m_pRigidStatic = m_pPhysics->createRigidStatic(PxTransform(XMMatrixToPxMat(worldMat)));
	
	PXCOLLIDER_DESC* pDesc = reinterpret_cast<PXCOLLIDER_DESC*>(pArg);
	_float fSF = 0.4f, fDF = 0.5f, fRest = 0.5f;
	if (pDesc)
	{
		fSF = pDesc->fStaticFriction;
		fDF = pDesc->fDynamicFriction;
		fRest = pDesc->fRestitution;
	}
	PxMaterial* mat = m_pPhysics->createMaterial(fSF, fDF, fRest);
	
	Attach_Shape(m_pRigidStatic, mat, vScale);
	m_pRigidStatic->userData = pDesc->pOwner;
	m_pCurrentScene->addActor(*m_pRigidStatic);

	return S_OK;
}

void CStaticCollider::Update(_float fTimeDelta)
{
	_matrix worldMat = m_pTransform->Get_WorldMatrix_XMMat();
	_matrix PrevMat = XMLoadFloat4x4(&m_PrevTransform);
	
	_bool bSame = true;
	for (_uint i = 0; i < 4; ++i)
	{
		if (!XMVector4Equal(worldMat.r[i], PrevMat.r[i]))
		{
			bSame = false;
			break;
		}
	}
	if (!bSame)
	{

		m_pRigidStatic->setGlobalPose(PxTransform(XMMatrixToPxMat(XMMatrixNormalize(worldMat))));
	}


	XMStoreFloat4x4(&m_PrevTransform, worldMat);
}

void CStaticCollider::Remove_Actor()
{
	m_pCurrentScene->removeActor(*m_pRigidStatic);
}

void CStaticCollider::Add_Actor()
{
	m_pCurrentScene->addActor(*m_pRigidStatic);
}

//bool CStaticCollider::Reshape()
//{
//	PxShape* pShape;
//	m_pRigidStatic->getShapes(&pShape, 1);
//	auto& Geo = pShape->getGeometry();
//	if (PxGeometryType::eTRIANGLEMESH == Geo.getType())
//	{
//		PxTriangleMeshGeometry TMG = static_cast<const PxTriangleMeshGeometry&>(Geo);
//		TMG.scale = PxMeshScale();
//	}
//
//	return false;
//}

shared_ptr<CStaticCollider> CStaticCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CStaticCollider
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CStaticCollider(pDevice, pContext) {}
	};

	shared_ptr<CStaticCollider> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CStaticCollider"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CStaticCollider::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CStaticCollider
	{
		MakeSharedEnabler(const CStaticCollider& rhs) : CStaticCollider(rhs) {}
	};

	shared_ptr<CStaticCollider> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CStaticCollider"));
		return nullptr;
	}

	return pInstance;
}

void CStaticCollider::Free()
{
	if (m_pRigidStatic)
		m_pCurrentScene->removeActor(*m_pRigidStatic);
}
