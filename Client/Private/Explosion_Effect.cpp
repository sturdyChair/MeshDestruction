#include "Explosion_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CExplosion_Effect::ObjID = TEXT("CExplosion_Effect");

CExplosion_Effect::CExplosion_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CExplosion_Effect::ObjID);
}

CExplosion_Effect::CExplosion_Effect(const CExplosion_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CExplosion_Effect::~CExplosion_Effect()
{
	Free();
}

HRESULT CExplosion_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Explosion_Scratch"), TEXT("../Bin/Resources/Effect/Explosion/T_VFX_tk_CellTex07_Scratch_01.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Explosion"), TEXT("../Bin/Resources/Effect/Explosion/T_VFX_tk_CellTex07_SmokeColor_01.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Explosion_Emissive"), TEXT("../Bin/Resources/Effect/Explosion/T_VFX_tk_CellTex07_Emissive_01.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Explosion_Alpha"), TEXT("../Bin/Resources/Effect/Explosion/T_VFX_tk_CellTex07_Alpha_01.png"), 1));




	return S_OK;
}

HRESULT CExplosion_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(48.f, 48.f, 1.f));

	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	m_vEffectColorPresets.push_back(_float4{ 1.0f,0.8f,0.4f,1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.7f, 0.7f,0.7f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.5f,0.5f,1.0f,1.f });
	m_vEffectBaseColor = _float4{ 1.0f,1.0f,1.0f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CExplosion_Effect::PriorityTick(_float fTimeDelta)
{


}

void CExplosion_Effect::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CExplosion_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CExplosion_Effect::Render()
{
	_float fTimeRatio = m_fEffectTimeAcc / m_fEffectTimeMax;
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	_float4x4 Temp;
	XMStoreFloat4x4(&Temp, XMMatrixScaling(fTimeRatio, fTimeRatio, fTimeRatio) * m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.2f) * 1.25f, 0.f));

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pAlphaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Alpha_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pEmissiveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Emissive_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::Explosion)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CExplosion_Effect::EffectStartCallBack()
{
}

void CExplosion_Effect::EffectEndCallBack()
{

}

void CExplosion_Effect::EffectSoftEnd()
{
}

void CExplosion_Effect::EffectHardEnd()
{
}

HRESULT CExplosion_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion_Scratch"), TEXT("Com_Noise_Texture")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion"), TEXT("Com_Texture")));

	m_pEmissiveTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion_Emissive"), TEXT("Com_EmissiveTexture")));
	m_pAlphaTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion_Alpha"), TEXT("Com_AlphaTexture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CExplosion_Effect> CExplosion_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CExplosion_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CExplosion_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CExplosion_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CExplosion_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CExplosion_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CExplosion_Effect
	{
		MakeSharedEnabler(const CExplosion_Effect& rhs) : CExplosion_Effect(rhs) {}
	};

	shared_ptr<CExplosion_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CExplosion_Effect"));
		assert(false);
	}

	return pInstance;
}

void CExplosion_Effect::Free()
{
}