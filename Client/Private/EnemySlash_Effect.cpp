#include "EnemySlash_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CEnemySlash_Effect::ObjID = TEXT("CEnemySlash_Effect");

CEnemySlash_Effect::CEnemySlash_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEnemySlash_Effect::ObjID);
}

CEnemySlash_Effect::CEnemySlash_Effect(const CEnemySlash_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CEnemySlash_Effect::~CEnemySlash_Effect()
{
	Free();
}

HRESULT CEnemySlash_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_EnemySlash_Effect_Slash_Enemy"), TEXT("../Bin/Resources/Effect/Slash/1/T_VFX_tk_swing_mt_03.png"), 1));

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_EnemySlash_Effect_Slash_Enemy_Scratch"), TEXT("../Bin/Resources/Effect/Slash/1/T_VFX_tk_scratch_swing_mt_03.png"), 1));

	return S_OK;
}

HRESULT CEnemySlash_Effect::Initialize(void* pArg)
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

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;
	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	m_vEffectBaseColor = _float4(0.2f, 2.f, 2.f, 5.f);
	SetColorPresetB(_float4(0.2f, 2.f, 2.f, 5.f));
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CEnemySlash_Effect::PriorityTick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_RETURN))
	{
		_float3 vScale = 
		m_pTransformCom->Get_Scale();
		vScale.x += fTimeDelta * 10.f;
		vScale.y += fTimeDelta * 10.f;
		vScale.z += fTimeDelta * 10.f;
		m_pTransformCom->Set_Scale(vScale);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_RSHIFT))
	{
		_float3 vScale =
			m_pTransformCom->Get_Scale();
		vScale.x -= fTimeDelta * 10.f;
		vScale.y -= fTimeDelta * 10.f;
		vScale.z -= fTimeDelta * 10.f;
		m_pTransformCom->Set_Scale(vScale);
	}
	
}

void CEnemySlash_Effect::Tick(_float fTimeDelta)
{
	//SetEffectLoop(true);
	//SetEffectTimeFreeze(false);
	//SetEffectColorBlendRate(1.f);

	//SetEffectBaseColorToColor();
	//m_vEffectBaseColor = _float4{ 0.2f,1.f,1.f,1.f };
	//mask.r* color1 mask.g* color2 mask.b* color3
	m_fAngle -= fTimeDelta * XM_2PI;

	XMStoreFloat4x4(&m_matRotation, XMMatrixAffineTransformation(XMVectorSet(1.f, 0.5f, 1.f, 0.f), XMVectorSet(0.f, -0.5f, 0.f, 1.f), XMQuaternionRotationAxis(XMLoadFloat3(&m_vRotationAxis), m_fAngle), XMVectorSet(0.f, 0.f, 0.f, 1.f)));
	EffectTick(fTimeDelta);
}

void CEnemySlash_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CEnemySlash_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 matTemp;
	XMStoreFloat4x4(&matTemp, XMLoadFloat4x4(&m_matRotation) * m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matTemp);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", powf(sin(m_fEffectTimeAcc * XM_PI), 2.f));

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::WeightBlendMonoColor)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEnemySlash_Effect::EffectStartCallBack()
{
}

void CEnemySlash_Effect::EffectEndCallBack()
{

}

void CEnemySlash_Effect::EffectSoftEnd()
{
}

void CEnemySlash_Effect::EffectHardEnd()
{
}

HRESULT CEnemySlash_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_EnemySlash_Effect_Slash_Enemy"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_EnemySlash_Effect_Slash_Enemy_Scratch"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEnemySlash_Effect> CEnemySlash_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEnemySlash_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEnemySlash_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CEnemySlash_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEnemySlash_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEnemySlash_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEnemySlash_Effect
	{
		MakeSharedEnabler(const CEnemySlash_Effect& rhs) : CEnemySlash_Effect(rhs) {}
	};

	shared_ptr<CEnemySlash_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEnemySlash_Effect"));
		assert(false);
	}

	return pInstance;
}

void CEnemySlash_Effect::Free()
{
}