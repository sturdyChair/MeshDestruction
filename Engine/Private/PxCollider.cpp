#include "..\Public\PxCollider.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

CPxCollider::CPxCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CPxCollider::CPxCollider(const CPxCollider& rhs)
	: CComponent(rhs),
	m_pPhysics{ rhs.m_pPhysics },
	m_pCurrentScene{ rhs.m_pCurrentScene }
{
}

CPxCollider::~CPxCollider()
{
	Free();
}

HRESULT CPxCollider::Initialize_Prototype()
{
	m_pPhysics = CGameInstance::Get_Instance()->Get_Physics();
	m_pCurrentScene = CGameInstance::Get_Instance()->Get_Scene();

	return S_OK;
}

HRESULT CPxCollider::Initialize(void* pArg)
{
	PXCOLLIDER_DESC* pDesc = ((PXCOLLIDER_DESC*)pArg);
	CGameObject* pOwner = pDesc->pOwner;
	m_pTransform = pOwner->Get_Transform();

	if(pDesc->strModelComTag != L"")
		m_pModel = dynamic_pointer_cast<CModel>(pOwner->Find_Component(pDesc->strModelComTag));
	else
		m_pModel = dynamic_pointer_cast<CModel>(pOwner->Find_Component_By_Type<CModel>());
	assert(m_pModel);
	
	return S_OK;
}

void CPxCollider::Update(_float fTimeDelta)
{
}

void CPxCollider::Attach_Shape(PxRigidActor* pActor, PxMaterial* mat, PxVec3 vScale)
{
	_uint iNumMesh = m_pModel->Get_NumMeshes();
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		auto pMesh = m_pModel->Get_Mesh(i);
		PxConvexMesh* pTM = pMesh->Get_ConvexMesh();
		if (!pTM)
		{
			pMesh->Cooking_ConvexMesh();
			pTM = pMesh->Get_ConvexMesh();
		}
		PxConvexMeshGeometry Geo{pTM};
		Geo.scale = vScale;
		PxShape* pShape = m_pPhysics->createShape(Geo, *mat);
		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		pShape->setSimulationFilterData(PxFilterData{ 1,1,1,1 });
		//pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		pActor->attachShape(*pShape);
	}
}

shared_ptr<CPxCollider> CPxCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CPxCollider
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPxCollider(pDevice, pContext) {}
	};

	shared_ptr<CPxCollider> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPxCollider"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CPxCollider::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CPxCollider
	{
		MakeSharedEnabler(const CPxCollider& rhs) : CPxCollider(rhs) {}
	};

	shared_ptr<CPxCollider> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPxCollider"));
		return nullptr;
	}

	return pInstance;
}

void CPxCollider::Free()
{
}
