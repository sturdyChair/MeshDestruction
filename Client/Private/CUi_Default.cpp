#include "CUi_Default.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_Default::ObjID = L"CUi_Default";

CUi_Default::CUi_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_Default::CUi_Default(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_Default::~CUi_Default()
{
}

HRESULT CUi_Default::Initialize_Prototype()
{
	Initialize_BeatmeterResource();
	Initialize_ScoreBackResource();
	Initialize_RankResource();
	Initialize_FinalResultBackResource();
	Initialize_StageResultResource();
	Initialize_GearResource();
	Initialize_BeatMarkerResource();
	Initialize_StoreResource();


	return S_OK;
}

HRESULT CUi_Default::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_Default::PriorityTick(_float fTimeDelta)
{
}

void CUi_Default::Tick(_float fTimeDelta)
{
}

void CUi_Default::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}


void CUi_Default::Initialize_BeatmeterResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Beatmeter",
			L"../Bin/Resources/Textures/Ui/BeatMeter/Beatmeter%d.png", 6));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatmeterBack",
			L"../Bin/Resources/Textures/Ui/BeatMeter/BeatmeterBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatmeterSpike",
			L"../Bin/Resources/Textures/Ui/BeatMeter/BeatmeterSpike%d.png", 6));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatmeterSpikeBack",
			L"../Bin/Resources/Textures/Ui/BeatMeter/BeatmeterSpikeBack%d.png", 4));
}

void CUi_Default::Initialize_BeatMarkerResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerBack",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerBackSmall",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerBackSmall.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerCat",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerCat.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerCircle",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerCircle.png"));
}

void CUi_Default::Initialize_RankResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Rank",
			L"../Bin/Resources/Textures/Ui/Rank/Rank%d.png", 6));
}

void CUi_Default::Initialize_ScoreBackResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ScoreBack",
			L"../Bin/Resources/Textures/Ui/Score/ScoreBack.png"));
}

void CUi_Default::Initialize_FinalResultBackResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultBack",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultString",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultRankString.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultBonus",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultBonus.png"));


	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultWord",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultWord.png"));
}

void CUi_Default::Initialize_StageResultResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Fade",
			L"../Bin/Resources/Textures/Ui/StageResult/CUi_Fade.png"));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Effect",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Effect.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Screen",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Screen.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Star",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Star.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Title",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Title.png"));
}

void CUi_Default::Initialize_HpResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_HpShadow",
			L"../Bin/Resources/Textures/Ui/Hp/.png"));
}

void CUi_Default::Initialize_GearResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Gear",
			L"../Bin/Resources/Textures/Ui/Gear/CUi_Gear.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_GearAdd",
			L"../Bin/Resources/Textures/Ui/Gear/GearAdd.png"));
}

void CUi_Default::Initialize_StoreResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBackDown",
			L"../Bin/Resources/Textures/Ui/Store/StoreBackDown.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBackUp",
			L"../Bin/Resources/Textures/Ui/Store/StoreBackUp.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreTitle",
			L"../Bin/Resources/Textures/Ui/Store/StoreTitle.png"));
		
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreOpen",
			L"../Bin/Resources/Textures/Ui/Store/StoreOpen%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SelectAttackStore",
			L"../Bin/Resources/Textures/Ui/Store/SelectAttackStore.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreHasSkill",
			L"../Bin/Resources/Textures/Ui/Store/StoreHasSkill.png"));
	
}


shared_ptr<CUi_Default> CUi_Default::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_Default
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_Default(pDevice, pContext) { }
	};

	shared_ptr<CUi_Default> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_Default::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_Default
	{
		MakeSharedEnabler(const CUi_Default& rhs) : CUi_Default(rhs) { }
	};

	shared_ptr<CUi_Default> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
