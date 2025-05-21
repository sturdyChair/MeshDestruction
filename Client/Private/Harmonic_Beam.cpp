#include "Harmonic_Beam.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CHarmonic_Beam::ObjID = TEXT("CHarmonic_Beam");

CHarmonic_Beam::CHarmonic_Beam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CHarmonic_Beam::ObjID);
}

CHarmonic_Beam::CHarmonic_Beam(const CHarmonic_Beam& rhs)
	: CEffect_Base(rhs)
{
}

CHarmonic_Beam::~CHarmonic_Beam()
{
	Free();
}

HRESULT CHarmonic_Beam::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Harmonic_Beam"), TEXT("../Bin/Resources/Effect/Beam/T_VFX_tk_beam_tc_01.dds"), 1));

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Harmonic_Beam_Scratch"), TEXT("../Bin/Resources/Effect/Beam/T_VFX_tk_scratch_16.png"), 1));

	return S_OK;
}

HRESULT CHarmonic_Beam::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(48.f, 192.f, 1.f));

	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));


	m_vEffectColorPresets.push_back({ 1.0f,1.0f,1.f,1.f });
	m_vEffectColorPresets.push_back({ 1.f,0.5f,0.2f,1.f });
	m_vEffectColorPresets.push_back({ 0.f,0.f,0.f,0.f });
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CHarmonic_Beam::PriorityTick(_float fTimeDelta)
{

}

void CHarmonic_Beam::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CHarmonic_Beam::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CHarmonic_Beam::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//_float4x4 matTemp;
	//XMStoreFloat4x4(&matTemp, XMLoadFloat4x4(&m_matRotation) * m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matTemp);
	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));


	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorR", &m_vEffectColorPresets[0], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[1], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[2], sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));
	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowXSpeed, m_fEffectUvFlowYSpeed };
	m_pShaderCom->Bind_RawValue("g_vUVFlowSpeed", &vFlowSpeed, sizeof(_float2));

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", powf(m_fEffectTimeAcc, 3.f));

	_float2 vUvMultiplier = _float2{ m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier };

	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUvMultiplier, sizeof(_float2));

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::WeightBlend)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CHarmonic_Beam::EffectStartCallBack()
{
}

void CHarmonic_Beam::EffectEndCallBack()
{

}

void CHarmonic_Beam::EffectSoftEnd()
{
}

void CHarmonic_Beam::EffectHardEnd()
{
}

HRESULT CHarmonic_Beam::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Harmonic_Beam"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Harmonic_Beam_Scratch"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CHarmonic_Beam> CHarmonic_Beam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CHarmonic_Beam
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CHarmonic_Beam(pDevice, pContext)
		{}
	};


	shared_ptr<CHarmonic_Beam> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHarmonic_Beam"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CHarmonic_Beam::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CHarmonic_Beam
	{
		MakeSharedEnabler(const CHarmonic_Beam& rhs) : CHarmonic_Beam(rhs) {}
	};

	shared_ptr<CHarmonic_Beam> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHarmonic_Beam"));
		assert(false);
	}

	return pInstance;
}

void CHarmonic_Beam::Free()
{
}