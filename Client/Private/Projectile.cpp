#include "Projectile.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "DynamicCollider.h"
#include "GameManager.h"
#include "CSG_Tester.h"
#include "PreBakedBrokenObj.h"

wstring CProjectile::ObjID = TEXT("CProjectile");

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CProjectile::CProjectile(const CProjectile& rhs)
	: CGameObject(rhs)
{
}

CProjectile::~CProjectile()
{
	Free();
}

HRESULT CProjectile::Initialize_Prototype()
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
	//Set_PoolSize(30);
	return S_OK;
}

HRESULT CProjectile::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_strModelTag = ((FDO_DESC*)pArg)->strModelTag;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_Dead();
	return S_OK;
}

void CProjectile::PriorityTick(_float fTimeDelta)
{
}

void CProjectile::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Dead();
		return;
	}
	_vector vDir = XMLoadFloat3(&m_vDir);
	_vector vPos = m_pTransformCom->Get_Position();
	_float4 vNextPos;
	XMStoreFloat4(&vNextPos, vPos + vDir * fTimeDelta);
	PxVec3 pxDir{ m_vDir.x,m_vDir.y,m_vDir.z};
	pxDir.normalize();
	PxSweepBuffer swBuf{};

	PxTransform start{ PxTransform(PxVec3{ vNextPos.x, vNextPos.y, vNextPos.z }) };
	_float fDist = m_pTransformCom->Get_Scale().x * 0.01f;
	GAMEINSTANCE->PxSweep(start, pxDir, XMVectorGetX(XMVector3Length(vDir)) * fTimeDelta, fDist, swBuf);
	for (_uint i = 0; i < swBuf.getNbAnyHits(); ++i)
	{
		auto& hit = swBuf.getAnyHit(i);
		if (!hit.actor->userData || hit.actor->userData == this)
			continue;
		CGameObject* pGO = (CGameObject*)hit.actor->userData;
		CCSG_Tester* pTester = dynamic_cast<CCSG_Tester*>(pGO);
		CPreBakedBrokenObj* pBroken = dynamic_cast<CPreBakedBrokenObj*>(pGO);
		if (pTester)
		{
			pTester->Simulate(hit, m_vDir);
		}
		else if (pBroken)
		{
			pBroken->Simulate(hit, m_vDir);
		}
		else if (pGO)
		{
			DamageInfo DInfo{};
			DInfo.pObjectOwner = this;
			DInfo.vCollisionDir = { m_vDir.x, m_vDir.y, m_vDir.z,0.f };
			DInfo.fAmountDamage = 0.f;
			DInfo.eAttackType = EAttackType::Special;
			pGO->Sweeped(hit, DInfo);
			Dead();
			return;
		}
	}

	m_pDynamicColliderCom->Update(fTimeDelta);

	m_pRigidBody->setKinematicTarget(start);
}

void CProjectile::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CProjectile::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		CGame_Manager::Get_Instance()->Get_DefaultRGBNoise()->Bind_ShaderResource(m_pShaderCom, "g_Texture");

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
		}


		m_pShaderCom->Begin((_uint)MESH_PASS::Emit);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CProjectile::Push_From_Pool(void* pArg)
{
	_float4x4 World = *(_float4x4*)(pArg);
	_matrix XMWorld = XMLoadFloat4x4(&World);
	XMWorld = XMMatrixNormalize(XMWorld);
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_pTransformCom->Set_WorldMatrix(XMWorld);
	m_pTransformCom->Set_Scale(vScale);

	_vector vDir = m_pTransformCom->Get_NormLook() * m_pTransformCom->Get_Speed();
	XMStoreFloat3(&m_vDir, vDir);

	XMWorld.r[3] += XMVector3Normalize(vDir);
	PxMat44 mat = XMMatrixToPxMat(XMWorld);
	GAMEINSTANCE->Get_Scene()->addActor(*m_pRigidBody);

	m_pTransformCom->Set_WorldMatrix(XMWorld);
	m_pRigidBody->setGlobalPose(PxTransform(mat));

	m_fLifeTime = 3.f;
}

void CProjectile::Dying()
{
	GAMEINSTANCE->Get_Scene()->removeActor(*m_pRigidBody);
}

HRESULT CProjectile::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_Model")));

	CPxCollider::PXCOLLIDER_DESC Desc{};
	Desc.pOwner = this;

	Desc.fStaticFriction = 0.4f;
	Desc.fDynamicFriction = 0.5f;
	Desc.fRestitution = 0.5f;

	m_pDynamicColliderCom = static_pointer_cast<CDynamicCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_DynamicCollider"), TEXT("Com_DynamicCollider"), &Desc));
	PxRigidDynamic* pRB = m_pDynamicColliderCom->Get_RigidBody();
	pRB->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	GAMEINSTANCE->Get_Scene()->removeActor(*pRB);
	m_pRigidBody = pRB;
	if (!m_pShaderCom || !m_pModelCom || !m_pDynamicColliderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CProjectile> CProjectile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CProjectile
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CProjectile(pDevice, pContext){}
	};


	shared_ptr<CProjectile> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CProjectile"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CProjectile::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CProjectile
	{
		MakeSharedEnabler(const CProjectile& rhs) : CProjectile(rhs)
		{
		}
	};


	shared_ptr<CProjectile> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CProjectile"));
		assert(false);
	}

	return pInstance;
}

void CProjectile::Free()
{
}
