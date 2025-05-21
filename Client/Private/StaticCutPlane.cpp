#include "StaticCutPlane.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Camera.h"
#include "StaticCutTester.h"

wstring CStaticCutPlane::ObjID = TEXT("CStaticCutPlane");

CStaticCutPlane::CStaticCutPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CStaticCutPlane::ObjID);
}

CStaticCutPlane::CStaticCutPlane(const CStaticCutPlane& rhs)
	: CEffect_Base(rhs)
{
}

CStaticCutPlane::~CStaticCutPlane()
{
	Free();
}

HRESULT CStaticCutPlane::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_T_VFX_yb_flare_02"), TEXT("../Bin/Resources/Effect/Flare/T_VFX_yb_flare_02.png"), 1));

	//m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_BigSlash_Effect_Slash_01_Scratch"), TEXT("../Bin/Resources/Effect/Slash/1/T_VFX_ym_swing_03.png"), 1));

	return S_OK;
}

HRESULT CStaticCutPlane::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(48.f, 48.f, 1.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	SetColorPresetRGB(_float4{ 1.f,0.1f,0.1f,1.f }, _float4{ 1.f,0.8f,0.1f,1.f }, _float4{ 0.8f,0.5f,1.0f,1.f });
	m_vEffectBaseColor = _float4{ 0.2f,0.5f,1.0f,1.f };
	EffectInit();
	shared_ptr<CStaticCutTester>* pspCuttable = (shared_ptr<CStaticCutTester>*)(pArg);
	m_pCuttable = { *pspCuttable };
	return S_OK;
}

void CStaticCutPlane::PriorityTick(_float fTimeDelta)
{
}

void CStaticCutPlane::Tick(_float fTimeDelta)
{
	if (m_pCuttable->Is_Dead())
	{
		Dead();
		return;
	}

	if (GAMEINSTANCE->Get_KeyDown(DIK_I))
	{
		m_bCutReady = !m_bCutReady;
	}
	if (m_bCutReady)
	{
		if (GAMEINSTANCE->Get_KeyPressing(DIK_F))
		{
			m_fLocalRotation += XM_PI * fTimeDelta;
		}
		if (GAMEINSTANCE->Get_KeyPressing(DIK_G))
		{
			m_fLocalRotation -= XM_PI * fTimeDelta;
		}
		_matrix CamMatrix = GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix_XMMat();
		_vector vCamLook = { 0.f,0.f,1.f,0.f }, vCamRight = { 1.f,0.f,0.f,0.f }, vCamUp = CamMatrix.r[1], vCamPos = CamMatrix.r[3];

		m_pTransformCom->Set_WorldMatrix(XMMatrixRotationAxis(vCamRight, XM_PIDIV2) * XMMatrixRotationAxis(vCamLook, m_fLocalRotation)
			* CamMatrix * XMMatrixTranslationFromVector(vCamUp * -0.01f));
		m_pTransformCom->Set_Scale({ 100.f,100.f,100.f, 0.f });

		if (GAMEINSTANCE->Get_KeyDown(DIK_C))
		{
			_float4 vPlane;
			XMStoreFloat4(&vPlane, XMPlaneFromPointNormal(m_pTransformCom->Get_Position(), m_pTransformCom->Get_NormLook()));

			m_pCuttable->Cut(vPlane);
		}

	}
}

void CStaticCutPlane::LateTick(_float fTimeDelta)
{
	if (m_bCutReady)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CStaticCutPlane::Render()
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

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::WeightBlendMonoColor)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;*/

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CStaticCutPlane::EffectStartCallBack()
{

}

void CStaticCutPlane::EffectEndCallBack()
{

}

void CStaticCutPlane::EffectSoftEnd()
{

}

void CStaticCutPlane::EffectHardEnd()
{

}

HRESULT CStaticCutPlane::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_yb_flare_02"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_yb_flare_02"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CStaticCutPlane> CStaticCutPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CStaticCutPlane
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CStaticCutPlane(pDevice, pContext)
		{
		}
	};


	shared_ptr<CStaticCutPlane> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CStaticCutPlane"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CStaticCutPlane::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CStaticCutPlane
	{
		MakeSharedEnabler(const CStaticCutPlane& rhs) : CStaticCutPlane(rhs) {}
	};

	shared_ptr<CStaticCutPlane> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CStaticCutPlane"));
		assert(false);
	}

	return pInstance;
}

void CStaticCutPlane::Free()
{
}