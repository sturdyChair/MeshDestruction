#include "Impact_Spark_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CImpact_Spark_Effect::ObjID = TEXT("CImpact_Spark_Effect");

CImpact_Spark_Effect::CImpact_Spark_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CImpact_Spark_Effect::ObjID);
}

CImpact_Spark_Effect::CImpact_Spark_Effect(const CImpact_Spark_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CImpact_Spark_Effect::~CImpact_Spark_Effect()
{
	Free();
}

HRESULT CImpact_Spark_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Spark"), TEXT("../Bin/Resources/Effect/Impact/Spark/Spark_0%d.png"), 4));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Spark_Scratch"), TEXT("../Bin/Resources/Effect/Impact/Spark/T_VFX_tk_scratch_01d_r.png"), 1));

	return S_OK;
}

HRESULT CImpact_Spark_Effect::Initialize(void* pArg)
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
	m_vEffectColorPresets.push_back(_float4{ 1.f,0.5f,0.0f,1.f });
	m_vEffectColorPresets.push_back(_float4{ 1.0f, 1.0f,0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.0f,0.0f,0.0f,0.f });
	m_vEffectBaseColor = _float4{ 1.0f,1.0f,1.0f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CImpact_Spark_Effect::PriorityTick(_float fTimeDelta)
{


}

void CImpact_Spark_Effect::Tick(_float fTimeDelta)
{
	m_Local.m[3][0] += m_vDir.x * fTimeDelta;
	m_Local.m[3][1] += m_vDir.y * fTimeDelta;
	m_Local.m[3][2] += m_vDir.z * fTimeDelta;

	EffectTick(fTimeDelta);
}

void CImpact_Spark_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CImpact_Spark_Effect::Render()
{
	_float fTimeRatio = m_fEffectTimeAcc / m_fEffectTimeMax;
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	_float4x4 Temp;
	XMStoreFloat4x4(&Temp, XMLoadFloat4x4(&m_Local)* m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorR", &m_vEffectColorPresets[0], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[1], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[2], sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));


	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));
	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowXSpeed, m_fEffectUvFlowYSpeed };
	m_pShaderCom->Bind_RawValue("g_vUVFlowSpeed", &vFlowSpeed, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_fEffectAlpha", m_bEffectAlphaLinkTimePer ? &m_fEffectTimePercentReverse : &m_fEffectAlpha, sizeof(_float));

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((fTimeRatio - (_int)(fTimeRatio)-0.2f) * 1.25f, 0.f));
	//m_pShaderCom->Bind_Float("g_fDissolveThreshold",  0.f);

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CImpact_Spark_Effect::EffectStartCallBack()
{
	m_iTexIdx = rand() % 4;
	_float fDegree = GAMEINSTANCE->Random_Float(0.f, XM_2PI);
	XMStoreFloat3(&m_vDir, XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMQuaternionRotationRollPitchYaw(0.f, 0.f, fDegree)));

	m_fSize = GAMEINSTANCE->Random_Float(1.f, 2.f);
	XMStoreFloat4x4(&m_Local, XMMatrixAffineTransformation(XMVectorSet(m_fSize * 0.25f, m_fSize, m_fSize,0.f),XMVectorSet(0.f,0.f,0.f,1.f), 
		XMQuaternionRotationRollPitchYaw(0.f,0.f,fDegree), XMVectorSetW(XMLoadFloat3(&m_vDir), 1.f)));
}

void CImpact_Spark_Effect::EffectEndCallBack()
{

}

void CImpact_Spark_Effect::EffectSoftEnd()
{
}

void CImpact_Spark_Effect::EffectHardEnd()
{
}

HRESULT CImpact_Spark_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Spark_Scratch"), TEXT("Com_Noise_Texture")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Spark"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CImpact_Spark_Effect> CImpact_Spark_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CImpact_Spark_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CImpact_Spark_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CImpact_Spark_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CImpact_Spark_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CImpact_Spark_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CImpact_Spark_Effect
	{
		MakeSharedEnabler(const CImpact_Spark_Effect& rhs) : CImpact_Spark_Effect(rhs) {}
	};

	shared_ptr<CImpact_Spark_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CImpact_Spark_Effect"));
		assert(false);
	}

	return pInstance;
}

void CImpact_Spark_Effect::Free()
{
}