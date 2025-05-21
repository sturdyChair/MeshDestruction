#include "Fire_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CFire_Effect::ObjID = TEXT("CFire_Effect");

CFire_Effect::CFire_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CFire_Effect::ObjID);
}

CFire_Effect::CFire_Effect(const CFire_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CFire_Effect::~CFire_Effect()
{
	Free();
}

HRESULT CFire_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_Fire"), TEXT("../Bin/Resources/Models/Effect/Fire/Fire.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("../Bin/Resources/Models/Effect/Fire/T_VFX_tk_scratch_03b.png"), 1));


	return S_OK;
}

HRESULT CFire_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	m_pTransformCom->Set_Scale(_float3{48.f,48.f,48.f});
	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.5f, 0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.9f, 0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	m_fEffectUvFlowXSpeed = 1.f;
	m_fEffectUvFlowYSpeed = 1.f;
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CFire_Effect::PriorityTick(_float fTimeDelta)
{
}

void CFire_Effect::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
}

void CFire_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CFire_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//_float4x4 Temp;
	//_float fScale = pow(min(m_fEffectTimePercent * 2.f, 1.f), 4.f);
	//XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale, fScale, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);


	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0);
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "", (_uint)i, aiTextureType_SPECULAR)))
		//	return E_FAIL;
		m_pShaderCom->Begin((_uint)EFFECT_PASS::Fire);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CFire_Effect::EffectStartCallBack()
{
}

void CFire_Effect::EffectEndCallBack()
{

}

void CFire_Effect::EffectSoftEnd()
{
}

void CFire_Effect::EffectHardEnd()
{
}

HRESULT CFire_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Fire"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("Com_Texture")));


	if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CFire_Effect> CFire_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFire_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFire_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CFire_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFire_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CFire_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFire_Effect
	{
		MakeSharedEnabler(const CFire_Effect& rhs) : CFire_Effect(rhs) {}
	};

	shared_ptr<CFire_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFire_Effect"));
		assert(false);
	}

	return pInstance;
}

void CFire_Effect::Free()
{
}
