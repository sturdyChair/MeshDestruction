#include "CUi_LoadingNote.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_LoadingNote::ObjID = L"CUi_LoadingNote";

CUi_LoadingNote::CUi_LoadingNote(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_LoadingNote::CUi_LoadingNote(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_LoadingNote::~CUi_LoadingNote()
{
}

HRESULT CUi_LoadingNote::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingNote1", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_note_1.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingNote2", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_note_2.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingNote3", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_note_3.png")
	);

	return S_OK;
}

HRESULT CUi_LoadingNote::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_fAppearTime = Arg->fAppearTime;
	m_fDisappearTime = Arg->fDisappearTime;

	return S_OK;
}

void CUi_LoadingNote::PriorityTick(_float fTimeDelta)
{
}

void CUi_LoadingNote::Tick(_float fTimeDelta)
{
	m_fAnimationTimer += fTimeDelta;

	if (m_fAnimationTimer >= m_fAppearTime && m_fAnimationTimer < m_fDisappearTime)
	{
		m_bVisible = true;
	}
	else if (m_fAnimationTimer >= m_fDisappearTime)
	{
		m_bVisible = false;
		m_fAnimationTimer = 0.f;
	}
}

void CUi_LoadingNote::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_LoadingNote::Render()
{
	if (!m_bVisible)
		return S_OK;

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

shared_ptr<CUi_LoadingNote> CUi_LoadingNote::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_LoadingNote
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_LoadingNote(pDevice, pContext) { }
	};

	shared_ptr<CUi_LoadingNote> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_LoadingNote::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_LoadingNote
	{
		MakeSharedEnabler(const CUi_LoadingNote& rhs) : CUi_LoadingNote(rhs) { }
	};

	shared_ptr<CUi_LoadingNote> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
