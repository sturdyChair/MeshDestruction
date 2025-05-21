#include "BackGround.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"


wstring CBackGround::ObjID = TEXT("CBackGround");

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: CGameObject{ rhs }
{
}

CBackGround::~CBackGround()
{
	Free();
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_BackGround"), TEXT("../Bin/Resources/Textures/Logo/Default.png"))
	);

	// If you want pooling set this
	Set_PoolSize(4);

	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));



	return S_OK;
}

void CBackGround::PriorityTick(_float fTimeDelta)
{
}

void CBackGround::Tick(_float fTimeDelta)
{
}

void CBackGround::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CBackGround::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader")));

	if (nullptr == m_pShaderCom) assert(false);

	/* For.Com_Texture */
	m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround"),
		TEXT("Com_Texture")));

	if (nullptr == m_pTextureCom) assert(false);

	/* For.Com_VIBuffer */
	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer")));

	if (nullptr == m_pVIBufferCom) assert(false);

	return S_OK;
}

shared_ptr<CBackGround> CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CBackGround
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBackGround(pDevice, pContext) { }
	};

	shared_ptr<CBackGround> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CBackGround::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CBackGround
	{
		MakeSharedEnabler(const CBackGround& rhs) : CBackGround(rhs) { }
	};

	shared_ptr<CBackGround> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBackGround"));
		assert(false);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();
}
