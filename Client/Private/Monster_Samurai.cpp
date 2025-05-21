#include "Monster_Samurai.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"


wstring CMonster_Samurai::ObjID = TEXT("CMonster_Samurai");

CMonster_Samurai::CMonster_Samurai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonsterBase{ pDevice, pContext }
{
}

CMonster_Samurai::CMonster_Samurai(const CMonster_Samurai& rhs)
	: CMonsterBase{ rhs }
{
}

CMonster_Samurai::~CMonster_Samurai()
{
	Free();
}

HRESULT CMonster_Samurai::Initialize_Prototype()
{
	_matrix		LocalTransformMatrix = XMMatrixIdentity();
	LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Monster_Samurai"), TEXT("../Bin/Resources/Models/Monsters/Samurai/Samurai.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CMonster_Samurai::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	PrepareCallbacks();

	return S_OK;
}

void CMonster_Samurai::PriorityTick(_float fTimeDelta)
{
}

void CMonster_Samurai::Tick(_float fTimeDelta)
{
	// Play Animation
	_float4x4 fmatResult;
	XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);

	if (m_pModelCom->IsRootAnim() && !m_pModelCom->IsForcedNonRoot())
	{
		_matrix matResult = XMLoadFloat4x4(&fmatResult);
		auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);

		_float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
		auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
	}
}

void CMonster_Samurai::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMonster_Samurai::Render()
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

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 4)	// аж╧Ф Mesh
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
		//    return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//    return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			continue;

		m_pShaderCom->Begin(0);

		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			continue;

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CMonster_Samurai::TakeDamage(const DamageInfo& damageInfo)
{
}

HRESULT CMonster_Samurai::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

	// Init Model and Matrices
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Monster_Samurai"), TEXT("Com_Model")));
	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

void CMonster_Samurai::PrepareCallbacks()
{
}

_bool CMonster_Samurai::IsAnimFinished()
{
	return _bool();
}

void CMonster_Samurai::SetAnim(const string& strAnimName, _bool isLoop)
{
}

void CMonster_Samurai::SetAnimHard(const string& strAnimName, _bool isLoop)
{
}

shared_ptr<CMonster_Samurai> CMonster_Samurai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMonster_Samurai
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Samurai(pDevice, pContext) {}
	};

	shared_ptr<CMonster_Samurai> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Samurai"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMonster_Samurai::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMonster_Samurai
	{
		MakeSharedEnabler(const CMonster_Samurai& rhs) : CMonster_Samurai(rhs) {}
	};

	shared_ptr<CMonster_Samurai> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Samurai"));
		assert(false);
	}

	return pInstance;
}

void CMonster_Samurai::Free()
{
}
