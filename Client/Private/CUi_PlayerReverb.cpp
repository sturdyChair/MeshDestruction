#include "CUi_PlayerReverb.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"


wstring CUi_PlayerReverb::ObjID = L"CUi_PlayerReverb";

CUi_PlayerReverb::CUi_PlayerReverb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_PlayerBar(pDevice, pContext)
{
}

CUi_PlayerReverb::CUi_PlayerReverb(const CUi_PlayerBar& rhs)
	:CUi_PlayerBar(rhs)
{
}

CUi_PlayerReverb::~CUi_PlayerReverb()
{
}

HRESULT CUi_PlayerReverb::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbGauge", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_PlayerGauge_ReverbBar_A_%d.png", 2)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbBar", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_PlayerGauge_EmptyReverb_A_%d.png", 2)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbOutline", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_PlayerGauge_ReverbOutline_B_0.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbShadow", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_ReverbBar_DropShadow_0.png")
	);

	return S_OK;
}

HRESULT CUi_PlayerReverb::Initialize(void* pArg)
{
	Initialize_Transform();

 	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_iTextureNum = Arg->iTextureNum;

	return S_OK;
}

void CUi_PlayerReverb::PriorityTick(_float fTimeDelta)
{

}

void CUi_PlayerReverb::Tick(_float fTimeDelta)
{
}

void CUi_PlayerReverb::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

shared_ptr<CUi_PlayerReverb> CUi_PlayerReverb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerReverb
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerReverb(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerReverb> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerReverb::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerReverb
	{
		MakeSharedEnabler(const CUi_PlayerReverb& rhs) : CUi_PlayerReverb(rhs) { }
	};

	shared_ptr<CUi_PlayerReverb> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
