#include "Loader.h"

#include "GameInstance.h"
#include "Load_Include.h"
#include "Component_Manager.h"
#include "SkyBox.h"

#include "ResourceWizard.h"

//#include "Monster.h"
//#include "Player.h"
//#include "Camera.h"
//#include "Effect.h"
//#include "Sky.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

CLoader::~CLoader()
{
	Free();
}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);
unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	return pLoader->Loading();
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT		hr{};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level();
		break;
	case LEVEL_TOOL:
		hr = Loading_For_Tool_Level();
		break;
	case LEVEL_TEST:
		hr = Loading_For_Test_Level();
		break;
	case LEVEL_ANIMTOOL:
		hr = Loading_For_AnimTool_Level();
		break;
	case LEVEL_MAPTOOL:
		hr = Loading_For_MapTool_Level();
		break;
	case LEVEL_EFFECTTOOL:
		hr = Loading_For_EffectTool_Level();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;
}

HRESULT CLoader::Load_Ui()
{
	if (FAILED(Load_Ui_Skill_Textures()))
		assert(false);

	if (FAILED(Load_Ui_Skill_Prototype()))
		assert(false);

	return S_OK;
}

HRESULT CLoader::Load_Ui_Skill_Textures()
{
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Hp_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Hp/hud_boss_health_bar_gauge.png")))))
		assert(false);*/

	return S_OK;
}

HRESULT CLoader::Load_Ui_Skill_Prototype()
{

	return S_OK;
}

HRESULT CLoader::Load_Object_Textures()
{

	return S_OK;
}

HRESULT CLoader::Load_Model()
{
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/Adrian/Adrian.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PistolGunnerWeapon"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/PistolGunnerWeapon/PistolGunnerWeapon.fbx", LocalTransformMatrix))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Loading..."));

	Load_Object_Textures();

	/*m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/Stage1.ogg", TEXT("Sound_BGM"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/Stage2.ogg", TEXT("Sound_BGM1"));*/

	//lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε� �� �Դϴ�."));




	//lstrcpy(m_szLoadingText, TEXT("Camera Load"));

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(TEXT("Prototype_GameObject_Camera_Tool"),
		CCamera_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("����(��) �ε� �� �Դϴ�."));

	Load_Ui();
	Load_Model();

	CResourceWizard::LoadResources(CBackGround::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	//ResourceWizard::LoadResources(CTestModel::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Tool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Tool Loading"));

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Test_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Level Loading"));

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GridBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/GridBox.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(TEXT("Prototype_GameObject_GridBox"),
		CGridBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	//CResourceWizard::LoadResources(CMonster_Saber::ObjID, m_eNextLevelID, m_pGameInstance, m_pDevice, m_pContext);

	//CResourceWizard::LoadResources(CMonster_Baboo::ObjID, m_eNextLevelID, m_pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CBarrier::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	//CResourceWizard::LoadResources(CMapObject_Static::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);


	//CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	//CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	//CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_ScoreSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	
	//CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CFloor::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFreeDynamicObject::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCSG_Tester::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CProjectile::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPreBakedBrokenObj::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCuttable::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCut_Plane::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWoundable::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CStaticCutTester::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CStaticCutPlane::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMyParticle::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDynamicFracture::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBreakable::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDamageApplier::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGeometryBrush::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);



	lstrcpy(m_szLoadingText, TEXT("Load Complete"));



	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_AnimTool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Tool Loading"));

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MapTool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Tool Loading"));

	CResourceWizard::LoadResources(CMapObject_Static::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMapObject_Anim::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_EffectTool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Tool Loading"));

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CResourceWizard::LoadResources(CTexture_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CTestEffect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBigSlash_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEnemySlash_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFlare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CImpact_Ring_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CImpact_Shock_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CImpact_Spark_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBeam_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSnap_Impact_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSnap_Font_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSmoke_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CConvexLens_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CStarSymbol_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CZap_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CExplosion_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBoom_Font_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CHarmonic_Beam::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLight_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);


	CResourceWizard::LoadResources(CLandImpact_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBarrier::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFire_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CParticle_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

shared_ptr<CLoader> CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	struct MakeSharedEnabler : public CLoader
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLoader(pDevice, pContext) { }
	};

	shared_ptr<CLoader> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		assert(false);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
