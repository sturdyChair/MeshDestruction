#include "Saber_Weapon.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"


wstring CSaber_Weapon::ObjID = TEXT("CSaber_Weapon");

CSaber_Weapon::CSaber_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CSaber_Weapon::CSaber_Weapon(const CSaber_Weapon& rhs)
    : CPartObject(rhs)
{
}

HRESULT CSaber_Weapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_matrix		LocalTransformMatrix = XMMatrixIdentity();
	LocalTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Saber_Weapon"), TEXT("../Bin/Resources/Models/Monsters/Saber/Weapon.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CSaber_Weapon::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
	m_pBoneMatrix = pDesc->pBoneMatrix;
	m_iLevelIndex = pDesc->iLevelIndex;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 위치 조정
	m_pTransformCom->Rotation(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90));

	return S_OK;
}

void CSaber_Weapon::PriorityTick(_float fTimeDelta)
{
}

void CSaber_Weapon::Tick(_float fTimeDelta)
{
	_matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);
	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);


	if (GAMEINSTANCE->Get_KeyPressing(DIK_Q))
		m_pTransformCom->Turn(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90.f), fTimeDelta);
	if (GAMEINSTANCE->Get_KeyPressing(DIK_E))
		m_pTransformCom->Turn(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(90.f), fTimeDelta);
	if (GAMEINSTANCE->Get_KeyPressing(DIK_R))
		m_pTransformCom->Turn(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(90.f), fTimeDelta);

}

void CSaber_Weapon::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CSaber_Weapon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		m_pShaderCom->Begin(0);

		if (FAILED(m_pModelCom->Bind_Buffers(i)))
			return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CSaber_Weapon::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Saber_Weapon"), TEXT("Com_Model")));
	
	return S_OK;
}

HRESULT CSaber_Weapon::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

    return S_OK;
}

shared_ptr<CSaber_Weapon> CSaber_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSaber_Weapon
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSaber_Weapon(pDevice, pContext) {}
	};

	shared_ptr<CSaber_Weapon> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSaber_Weapon"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CSaber_Weapon::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSaber_Weapon
	{
		MakeSharedEnabler(const CSaber_Weapon& rhs) : CSaber_Weapon(rhs) {}
	};

	shared_ptr<CSaber_Weapon> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSaber_Weapon"));
		assert(false);
	}
	return pInstance;
}

void CSaber_Weapon::Free()
{
}

CSaber_Weapon::~CSaber_Weapon()
{
	Free();
}
