#include "CUi_PlayerGuiter.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"


wstring CUi_PlayerGuiter::ObjID = L"CUi_PlayerGuiter";

CUi_PlayerGuiter::CUi_PlayerGuiter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_PlayerBar(pDevice, pContext)
{
}

CUi_PlayerGuiter::CUi_PlayerGuiter(const CUi_PlayerBar& rhs)
	:CUi_PlayerBar(rhs)
{
}

CUi_PlayerGuiter::~CUi_PlayerGuiter()
{
}

HRESULT CUi_PlayerGuiter::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerGuiter", L"../Bin/Resources/Textures/Ui/PlayerBar/Guiter/T_HUD_LifeGauge_GuiterBack_Color.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerGuiterBackground", L"../Bin/Resources/Textures/Ui/PlayerBar/Guiter/T_HUD_PlayerGauge_GuiterBack.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerGuiterShadow", L"../Bin/Resources/Textures/Ui/PlayerBar/Guiter/T_HUD_GuiterBody_DropShadow.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerBarLock", L"../Bin/Resources/Textures/Ui/PlayerBar/Lock/T_HUD_PlayerGauge_Lock.png")
	);

	return S_OK;
}

HRESULT CUi_PlayerGuiter::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_PlayerGuiter::PriorityTick(_float fTimeDelta)
{

}

void CUi_PlayerGuiter::Tick(_float fTimeDelta)
{
	
}

void CUi_PlayerGuiter::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

shared_ptr<CUi_PlayerGuiter> CUi_PlayerGuiter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerGuiter
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerGuiter(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerGuiter> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerGuiter::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerGuiter
	{
		MakeSharedEnabler(const CUi_PlayerGuiter& rhs) : CUi_PlayerGuiter(rhs) { }
	};

	shared_ptr<CUi_PlayerGuiter> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
