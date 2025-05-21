#include "Barrier.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding.h"
#include "Bounding_Sphere.h"

wstring CBarrier::ObjID = TEXT("CBarrier");

CBarrier::CBarrier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CBarrier::ObjID);
}

CBarrier::CBarrier(const CBarrier& rhs)
	: CEffect_Base(rhs)
{
}

CBarrier::~CBarrier()
{
	Free();
}

HRESULT CBarrier::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_Barrier"), TEXT("../Bin/Resources/Models/Effect/Barrier/Sphere_Noise.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 1.f, 0.f,  0.f,  0.f,
																																											0.f,  1.f, 0.f,  0.f,
																																											0.f,  0.f,  1.f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);
	//
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Barrier_Grid"), TEXT("../Bin/Resources/Models/Effect/Barrier/T_VFX_tk_Barrier_03.png"), 1));

	return S_OK;
}

HRESULT CBarrier::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	//m_pTransformCom->Set_Scale(_float3{48.f,48.f,48.f});
	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.8f, 0.8f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.4f, 0.2f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	m_vEffectBaseColor = _float4{ 0.25f,0.6f,0.6f,0.5f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	m_bEffectLoop = true;
	return S_OK;
}

void CBarrier::PriorityTick(_float fTimeDelta)
{
}

void CBarrier::Tick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
	EffectTick(fTimeDelta);
}

void CBarrier::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CBarrier::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);
	m_pShaderCom->Bind_Vector4("g_vCamPos", &m_pGameInstance.lock()->Get_CamPosition_Float4());

	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Effect_Texture_01", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)EFFECT_PASS::Barrier);

		m_pModelCom->Render(i);
	}
	m_pCollider->Render();
	return S_OK;
}

void CBarrier::EffectStartCallBack()
{
}

void CBarrier::EffectEndCallBack()
{

}

void CBarrier::EffectSoftEnd()
{
}

void CBarrier::EffectHardEnd()
{
}

HRESULT CBarrier::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Barrier"), TEXT("Com_Model")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Barrier_Grid"), TEXT("Com_Texture")));


	CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
	sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
	sphereDesc.fRadius = 1.0f;
	m_pCollider =
		dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
	m_pCollider->Set_Owner(shared_from_this());
	m_pCollider->Set_ColliderTag(TEXT("Barrier"));

	if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom || !m_pCollider)
		assert(false);

	return S_OK;
}

shared_ptr<CBarrier> CBarrier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBarrier
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBarrier(pDevice, pContext)
		{}
	};


	shared_ptr<CBarrier> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBarrier"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CBarrier::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBarrier
	{
		MakeSharedEnabler(const CBarrier& rhs) : CBarrier(rhs) {}
	};

	shared_ptr<CBarrier> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBarrier"));
		assert(false);
	}

	return pInstance;
}

void CBarrier::Free()
{
}
