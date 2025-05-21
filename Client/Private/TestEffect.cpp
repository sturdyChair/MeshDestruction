#include "TestEffect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CTestEffect::ObjID = TEXT("CTestEffect");

CTestEffect::CTestEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
}

CTestEffect::CTestEffect(const CTestEffect& rhs)
	: CEffect_Base(rhs)
{
}

CTestEffect::~CTestEffect()
{
	Free();
}

HRESULT CTestEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_Slash_01"), TEXT("../Bin/Resources/Models/Effect/Test/Slash01.fbx"), MODEL_TYPE::NONANIM, _float4x4{  1.0f, 0.f,  0.f,  0.f,
																																										    0.f,  1.0f, 0.f,  0.f,
																																											0.f,  0.f,  1.0f, 0.f,
																																										    0.f,  0.f,  0.f,  1.f })
	);

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("../Bin/Resources/Models/Effect/Test/SlashNoise%d.png"),2));

	return S_OK;
}

HRESULT CTestEffect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	
	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);
	
	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	
	return S_OK;
}

void CTestEffect::PriorityTick(_float fTimeDelta)
{
}

void CTestEffect::Tick(_float fTimeDelta)
{
	SetEffectLoop(true);
	SetEffectTimeFreeze(false);
	SetEffectColorBlendRate(1.f);

	SetEffectBaseColorToColor(_float4(0.29f, 0.58f, 0.73f, 1.f));



	EffectTick(fTimeDelta);
}

void CTestEffect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CTestEffect::Render()
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
	
	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
	
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));

	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 1)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)EFFECT_PASS::EffectCCW);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CTestEffect::EffectStartCallBack()
{
}

void CTestEffect::EffectEndCallBack()
{

}

void CTestEffect::EffectSoftEnd()
{
}

void CTestEffect::EffectHardEnd()
{
}

HRESULT CTestEffect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Slash_01"), TEXT("Com_Model")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	
	if (!m_pShaderCom || !m_pModelCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CTestEffect> CTestEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CTestEffect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTestEffect(pDevice, pContext)
		{}
	};


	shared_ptr<CTestEffect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTestEffect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CTestEffect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CTestEffect
	{
		MakeSharedEnabler(const CTestEffect& rhs) : CTestEffect(rhs) {}
	};

	shared_ptr<CTestEffect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTestEffect"));
		assert(false);
	}

	return pInstance;
}

void CTestEffect::Free()
{
}
