#include "Floor.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "GameManager.h"
#include "StaticCollider.h"
#include <iostream>

wstring CFloor::ObjID = TEXT("CFloor");

CFloor::CFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CFloor::CFloor(const CFloor& rhs)
	: CGameObject(rhs)
{
}

CFloor::~CFloor()
{
	Free();
}

HRESULT CFloor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Floor"), TEXT("../Bin/Resources/Models/Cube/Floor.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																					   0.f,	 0.01f, 0.f,  0.f,
																																					   0.f,  0.f,  0.01f, 0.f,
																																					   0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CFloor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CFloor::PriorityTick(_float fTimeDelta)
{
}

void CFloor::Tick(_float fTimeDelta)
{

	m_pStaticColliderCom->Update(fTimeDelta);
}

void CFloor::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
}

HRESULT CFloor::Render()
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

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SHININESS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultOrm()->Bind_ShaderResource(m_pShaderCom, "g_ATOS_Texture");
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		{
			CGame_Manager::Get_Instance()->Get_DefaultNorm()->Bind_ShaderResource(m_pShaderCom, "g_Normal_Texture");
		}


		m_pShaderCom->Begin((_uint)MESH_PASS::Default);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CFloor::OnPxContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPair, CGameObject* pOther)
{
	//cout << "Floor Contact\n";
}

HRESULT CFloor::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Floor"), TEXT("Com_Model")));
	CPxCollider::PXCOLLIDER_DESC Desc{};
	Desc.pOwner = this;

	Desc.fStaticFriction = 0.4f;
	Desc.fDynamicFriction = 0.5f;
	Desc.fRestitution = 0.5f;

	m_pStaticColliderCom = static_pointer_cast<CStaticCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StaticCollider"), TEXT("Com_StaticCollider"), &Desc));

	if (!m_pShaderCom || !m_pModelCom || !m_pStaticColliderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CFloor> CFloor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFloor
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFloor(pDevice, pContext)
		{
		}
	};


	shared_ptr<CFloor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFloor"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CFloor::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFloor
	{
		MakeSharedEnabler(const CFloor& rhs) : CFloor(rhs)
		{
		}
	};


	shared_ptr<CFloor> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFloor"));
		assert(false);
	}

	return pInstance;
}

void CFloor::Free()
{
}
