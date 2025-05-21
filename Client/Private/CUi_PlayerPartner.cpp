#include "CUi_PlayerPartner.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"


wstring CUi_PlayerPartner::ObjID = L"CUi_PlayerPartner";

CUi_PlayerPartner::CUi_PlayerPartner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_PlayerBar(pDevice, pContext)
{
}

CUi_PlayerPartner::CUi_PlayerPartner(const CUi_PlayerBar& rhs)
	:CUi_PlayerBar(rhs)
{
}

CUi_PlayerPartner::~CUi_PlayerPartner()
{
}

HRESULT CUi_PlayerPartner::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerPartnerPeppermint", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_new_Peppermint_01.png")
	);

	return S_OK;
}

HRESULT CUi_PlayerPartner::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_iTextureNum = Arg->iTextureNum;

	return S_OK;
}

void CUi_PlayerPartner::PriorityTick(_float fTimeDelta)
{

}

void CUi_PlayerPartner::Tick(_float fTimeDelta)
{

}

void CUi_PlayerPartner::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

shared_ptr<CUi_PlayerPartner> CUi_PlayerPartner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerPartner
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerPartner(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerPartner> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerPartner::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerPartner
	{
		MakeSharedEnabler(const CUi_PlayerPartner& rhs) : CUi_PlayerPartner(rhs) { }
	};

	shared_ptr<CUi_PlayerPartner> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
