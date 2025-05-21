#include "DamageApplier.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding.h"
#include "Bounding_Sphere.h"
#include "Breakable.h"
#include "Camera.h"
#include "Mesh.h"

wstring CDamageApplier::ObjID = TEXT("CDamageApplier");

CDamageApplier::CDamageApplier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CDamageApplier::ObjID);
}

CDamageApplier::CDamageApplier(const CDamageApplier& rhs)
	: CEffect_Base(rhs)
{
}

CDamageApplier::~CDamageApplier()
{
	Free();
}

HRESULT CDamageApplier::Initialize_Prototype()
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

HRESULT CDamageApplier::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_pModelCom->Get_Mesh(0)->Cooking_ConvexMesh();

	m_Geometry = { m_pModelCom->Get_Mesh(0)->Get_ConvexMesh()};

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	m_pTransformCom->Set_Scale(_float3{5.f,5.f,5.f});
	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.8f, 0.8f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.4f, 0.2f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	m_vEffectBaseColor = _float4{ 0.25f,0.6f,0.6f,0.5f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	m_bEffectLoop = true;
	return S_OK;
}

void CDamageApplier::PriorityTick(_float fTimeDelta)
{
	if (GAMEINSTANCE->Get_KeyDown(DIK_K))
	{
		m_bActive = !m_bActive;
	}
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CDamageApplier::Tick(_float fTimeDelta)
{
	_float3 vScale = m_pTransformCom->Get_Scale();
	PxVec3 pxScale{ vScale.x,vScale.y,vScale.z };
	m_Geometry.scale = pxScale;
	EffectTick(fTimeDelta);
	if (m_bActive)
	{
		_matrix World = GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix_XMMat();
		World.r[3] += XMVector3Normalize(World.r[2]) * (vScale.z + 2.5f);
		m_pTransformCom->Set_WorldMatrix(XMMatrixScaling(vScale.x,vScale.y,vScale.z) * XMMatrixNormalize(World));
	}
}

void CDamageApplier::LateTick(_float fTimeDelta)
{
	if (m_bActive) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		//if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
		//{
		//	POINT pPt = {};
		//	GetCursorPos(&pPt);
		//	ScreenToClient(g_hWnd, &pPt);
		//	_float4 vPos = GAMEINSTANCE->Pick_Position(pPt.x, pPt.y);
		//	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
		//	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
		//}
		if (GAMEINSTANCE->Get_KeyDown(DIK_C))
		{
			Find_Breakable();
		}
	}
	
}

HRESULT CDamageApplier::Render()
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
	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Effect_Texture_01", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)EFFECT_PASS::Barrier);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CDamageApplier::Apply_Damage(shared_ptr<CBreakable> pTarget)
{
	PxTransform pose{XMMatrixToPxMat(XMMatrixNormalize(m_pTransformCom->Get_WorldMatrix_XMMat()))};
	pTarget->Apply_Damage_Shape(m_Geometry, pose);
}

void CDamageApplier::Find_Breakable()
{
	PxTransform pose{ XMMatrixToPxMat(XMMatrixNormalize(m_pTransformCom->Get_WorldMatrix_XMMat())) };
	auto pScene = GAMEINSTANCE->Get_Scene();
	PxSweepBuffer sweepBuffer{};
	if (pScene->sweep(m_Geometry, pose, { 0.f,0.f,1.f }, 0.f, sweepBuffer))
	{
		_uint iNumHit = sweepBuffer.getNbAnyHits();
		for (_uint i = 0; i < iNumHit; ++i)
		{
			CGameObject* pRhs = (CGameObject*)sweepBuffer.getAnyHit(i).actor->userData;
			if (pRhs)
			{
				CBreakable* pBreakable = dynamic_cast<CBreakable*>(pRhs);
				if (pBreakable)
				{
					pBreakable->Apply_Damage_Shape(m_Geometry, pose);
				}
			}
		}
	}
}



void CDamageApplier::EffectStartCallBack()
{
}

void CDamageApplier::EffectEndCallBack()
{

}

void CDamageApplier::EffectSoftEnd()
{
}

void CDamageApplier::EffectHardEnd()
{
}

HRESULT CDamageApplier::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Barrier"), TEXT("Com_Model")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Barrier_Grid"), TEXT("Com_Texture")));



	if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CDamageApplier> CDamageApplier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CDamageApplier
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDamageApplier(pDevice, pContext)
		{
		}
	};


	shared_ptr<CDamageApplier> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDamageApplier"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CDamageApplier::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CDamageApplier
	{
		MakeSharedEnabler(const CDamageApplier& rhs) : CDamageApplier(rhs) {}
	};

	shared_ptr<CDamageApplier> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDamageApplier"));
		assert(false);
	}

	return pInstance;
}

void CDamageApplier::Free()
{
}
