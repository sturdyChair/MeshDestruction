#include "CUi_PlayerHP.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"


wstring CUi_PlayerHP::ObjID = L"CUi_PlayerHP";

CUi_PlayerHP::CUi_PlayerHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_PlayerBar(pDevice, pContext)
{
}

CUi_PlayerHP::CUi_PlayerHP(const CUi_PlayerBar& rhs)
	:CUi_PlayerBar(rhs)
{
}

CUi_PlayerHP::~CUi_PlayerHP()
{
}

HRESULT CUi_PlayerHP::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerHPGauge", L"../Bin/Resources/Textures/Ui/PlayerBar/HP/T_HealthBar_Bar_7.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerDamageGauge", L"../Bin/Resources/Textures/Ui/PlayerBar/HP/T_HealthBar_DamageEffect_7.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerHPBar", L"../Bin/Resources/Textures/Ui/PlayerBar/HP/T_HealthBar_Bg_7.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerHPBarShadow", L"../Bin/Resources/Textures/Ui/PlayerBar/HP/T_HealthBar_DropShadow_7.png")
	);

	return S_OK;
}

HRESULT CUi_PlayerHP::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	return S_OK;
}

void CUi_PlayerHP::PriorityTick(_float fTimeDelta)
{

}

void CUi_PlayerHP::Tick(_float fTimeDelta)
{
	
}

void CUi_PlayerHP::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

shared_ptr<CUi_PlayerHP> CUi_PlayerHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerHP
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerHP(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerHP> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerHP::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerHP
	{
		MakeSharedEnabler(const CUi_PlayerHP& rhs) : CUi_PlayerHP(rhs) { }
	};

	shared_ptr<CUi_PlayerHP> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
