#include "ConvexLens_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CConvexLens_Effect::ObjID = TEXT("CConvexLens_Effect");

CConvexLens_Effect::CConvexLens_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CConvexLens_Effect::ObjID);
}

CConvexLens_Effect::CConvexLens_Effect(const CConvexLens_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CConvexLens_Effect::~CConvexLens_Effect()
{
	Free();
}

HRESULT CConvexLens_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CConvexLens_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(_float3(0.5f, 0.25f, 1.f));

	_float4 vPos = _float4(0.5f, 0.5f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	m_vEffectColorPresets.push_back(_float4{ 1.f,1.0f,1.0f,5.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.5f,1.0f, 5.f });
	m_vEffectColorPresets.push_back(_float4{ 0.5f,0.5f,1.0f,5.f });
	m_vEffectBaseColor = _float4{ 1.0f,1.0f,1.0f,1.f };
	m_bEffectAlphaLinkTimePer = true;
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CConvexLens_Effect::PriorityTick(_float fTimeDelta)
{


}

void CConvexLens_Effect::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CConvexLens_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_DISTORTION, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CConvexLens_Effect::Render()
{
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_pShaderCom->Bind_Float( "g_fConvexRadius", vScale.y);
	_vector vPos = m_pTransformCom->Get_Position();
	_float2 vCenter = _float2{ vPos.m128_f32[0], vPos.m128_f32[1]};
	m_pShaderCom->Bind_RawValue("g_vConvexCenter", &vCenter, sizeof(_float2));
	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	m_pShaderCom->Bind_Float("g_fConvexPower", m_bEffectAlphaLinkTimePer ? m_fEffectTimePercentReverse * vScale.x : m_fEffectAlpha);


	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::ConvexLens)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CConvexLens_Effect::EffectStartCallBack()
{
}

void CConvexLens_Effect::EffectEndCallBack()
{

}

void CConvexLens_Effect::EffectSoftEnd()
{
}

void CConvexLens_Effect::EffectHardEnd()
{
}

HRESULT CConvexLens_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CConvexLens_Effect> CConvexLens_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CConvexLens_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CConvexLens_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CConvexLens_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CConvexLens_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CConvexLens_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CConvexLens_Effect
	{
		MakeSharedEnabler(const CConvexLens_Effect& rhs) : CConvexLens_Effect(rhs) {}
	};

	shared_ptr<CConvexLens_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CConvexLens_Effect"));
		assert(false);
	}

	return pInstance;
}

void CConvexLens_Effect::Free()
{
}