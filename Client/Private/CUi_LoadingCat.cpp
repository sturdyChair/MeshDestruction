#include "CUi_LoadingCat.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_LoadingCat::ObjID = L"CUi_LoadingCat";

CUi_LoadingCat::CUi_LoadingCat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_LoadingCat::CUi_LoadingCat(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_LoadingCat::~CUi_LoadingCat()
{
}

HRESULT CUi_LoadingCat::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingCat", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_808.png")
	);

	return S_OK;
}

HRESULT CUi_LoadingCat::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	return S_OK;
}

void CUi_LoadingCat::PriorityTick(_float fTimeDelta)
{
}

void CUi_LoadingCat::Tick(_float fTimeDelta)
{
}

void CUi_LoadingCat::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_LoadingCat::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin(0)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	return S_OK;
}

shared_ptr<CUi_LoadingCat> CUi_LoadingCat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_LoadingCat
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_LoadingCat(pDevice, pContext) { }
	};

	shared_ptr<CUi_LoadingCat> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_LoadingCat::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_LoadingCat
	{
		MakeSharedEnabler(const CUi_LoadingCat& rhs) : CUi_LoadingCat(rhs) { }
	};

	shared_ptr<CUi_LoadingCat> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
