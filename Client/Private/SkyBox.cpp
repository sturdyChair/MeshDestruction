#include "SkyBox.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"


CSkyBox::CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: CGameObject(rhs)
{
}

CSkyBox::~CSkyBox()
{
	Free();
}

HRESULT CSkyBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CSkyBox::PriorityTick(_float fTimeDelta)
{
}

void CSkyBox::Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance.lock()->Get_CamPosition_Vector());

}

void CSkyBox::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_PRIORITY, shared_from_this());
}

HRESULT CSkyBox::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader")));

	/* For.Com_Texture */
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"), TEXT("Com_Texture")));

	/* For.Com_VIBuffer */
	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Cube>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pVIBufferCom)
		assert(false);

	return S_OK;
}

shared_ptr<CSkyBox> CSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSkyBox
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkyBox(pDevice,pContext)
		{}
	};


	shared_ptr<CSkyBox> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkyBox"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CSkyBox::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSkyBox
	{
		MakeSharedEnabler(const CSkyBox& rhs) : CSkyBox(rhs)
		{}
	};


	shared_ptr<CSkyBox> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkyBox"));
		assert(false);
	}

	return pInstance;
}

void CSkyBox::Free()
{
}
