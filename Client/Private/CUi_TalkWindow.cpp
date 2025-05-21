#include "CUi_TalkWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"


wstring CUi_TalkWindow::ObjID = L"CUi_TalkWindow";

CUi_TalkWindow::CUi_TalkWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_TalkSystem(pDevice, pContext)
{
}

CUi_TalkWindow::CUi_TalkWindow(const CUi_TalkSystem& rhs)
	:CUi_TalkSystem(rhs)
{
}

CUi_TalkWindow::~CUi_TalkWindow()
{
}

HRESULT CUi_TalkWindow::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkBlackBoard", L"../Bin/Resources/Textures/Ui/Talk/Window/BlackBoard.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindow0", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_00.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindow1", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_01.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindow2", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_02.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindow3", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_03.png")
	);

	return S_OK;
}

HRESULT CUi_TalkWindow::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_iTextureNum = Arg->iTextureNum;

	Set_Font(Arg->Font);

	return S_OK;
}

void CUi_TalkWindow::PriorityTick(_float fTimeDelta)
{

}

void CUi_TalkWindow::Tick(_float fTimeDelta)
{
}

void CUi_TalkWindow::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_TalkWindow::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	RenderFont(m_FontDesc);

	return S_OK;
}

void CUi_TalkWindow::Set_Font(FontDescInfo Info)
{
	m_FontDesc = Info;
}

shared_ptr<CUi_TalkWindow> CUi_TalkWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TalkWindow
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TalkWindow(pDevice, pContext) { }
	};

	shared_ptr<CUi_TalkWindow> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TalkWindow::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TalkWindow
	{
		MakeSharedEnabler(const CUi_TalkWindow& rhs) : CUi_TalkWindow(rhs) { }
	};

	shared_ptr<CUi_TalkWindow> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
