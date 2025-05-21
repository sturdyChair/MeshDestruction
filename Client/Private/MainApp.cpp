#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "GameObject.h"

#include "PlayerManager.h"
#include "GameManager.h"
#include "Imgui_Manager.h"
#include "CUi_Mgr.h"

#include "Component_Manager.h"
#include "Load_Include.h"

#include "ResourceWizard.h"
#include "Beat_Manager.h"

#include "CombatManager.h"
#include "Effect_Manager.h"

#include "BackGround.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{

}

CMainApp::~CMainApp()
{
	Free();
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.hInstance = g_hInst;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	m_pImguiManager = CImgui_Manager::Get_Instance();
	m_pImguiManager->Initialize(g_hWnd, m_pDevice, m_pContext);

	m_pPlayerManager = CPlayer_Manager::Get_Instance();
	m_pPlayerManager->Initialize(m_pDevice, m_pContext);

	m_pGameManager = CGame_Manager::Get_Instance();
	m_pGameManager->Initialize(m_pDevice, m_pContext);

	m_pCUiMgr = CUi_Mgr::Get_Instance();
	m_pCUiMgr->Initialize(m_pDevice, m_pContext);

	m_pBeat_Manager = CBeat_Manager::Get_Instance();
	m_pBeat_Manager->Initialize();

	m_pCombat_Manager = CCombat_Manager::Get_Instance();
	m_pCombat_Manager->Initialize(m_pDevice, m_pContext);

	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	m_pEffect_Manager->Initialize(m_pDevice, m_pContext);

	if (FAILED(Ready_Prototoype_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_Prototoype_Objects()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pBeat_Manager->Tick(fTimeDelta);
	m_pPlayerManager->Tick_Engine(fTimeDelta);
	m_pGameManager->Tick_Engine(fTimeDelta);
	m_pCombat_Manager->Tick_Engine(fTimeDelta);
	m_pCUiMgr->Tick_Engine(fTimeDelta);
	m_pGameInstance->Tick_Engine(fTimeDelta);
	m_pImguiManager->Tick_Engine(fTimeDelta);
	m_pEffect_Manager->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

	m_pImguiManager->Imgui_Render();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	CResourceWizard::LoadResources(CUi_LoadingCat::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_LoadingNote::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototoype_Component_ForStatic()
{
	/*Static Collideres*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StaticCollider"),
		CStaticCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_DynamicCollider"),
		CDynamicCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*Static Shader*/

	///* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//	return E_FAIL;
	
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UiGauge"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UiGauge.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	//	return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;
	
	///* For.Prototype_Component_Shader_VtxRectInstance */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
	//	return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"),
		CCharacterController::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, CModel_Cutter::s_PrototypeTag,
		CModel_Cutter::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/MGRR_Cutter.hlsl")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Cutter_Controller"),
		CCutter_Controller::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_PxRagDoll"),
		CPxRagDoll::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_MultiCutterController"),
		CMultiCutterController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, CSnapper::s_PrototypeTag,
		CSnapper::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Mesh_Snap.hlsl")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, CStatic_Cutter::s_PrototypeTag,
		CStatic_Cutter::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Static_Cutter.hlsl")))))
		return E_FAIL;


	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototoype_Objects()
{
#pragma region OLD
#pragma region JAEHWI
	if (FAILED(m_pGameInstance->Add_Prototype(CBackGround::ObjID, CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTestModel::ObjID, CTestModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion //JAEHWI

#pragma region BEOMSEOK
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_LoadingCat::ObjID, CUi_LoadingCat::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_LoadingNote::ObjID, CUi_LoadingNote::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerBar::ObjID, CUi_PlayerBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerGuiter::ObjID, CUi_PlayerGuiter::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerHP::ObjID, CUi_PlayerHP::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerReverb::ObjID, CUi_PlayerReverb::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerPartner::ObjID, CUi_PlayerPartner::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TalkSystem::ObjID, CUi_TalkSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TalkCharacter::ObjID, CUi_TalkCharacter::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TalkWindow::ObjID, CUi_TalkWindow::Create(m_pDevice, m_pContext))))
		assert(false);
#pragma endregion //BEOMSEOK

#pragma region JEONGRAE
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_Default::ObjID, CUi_Default::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_RankSystem::ObjID, CUi_RankSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_ComboNum::ObjID, CUi_ComboNum::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_ComboWordHits::ObjID, CUi_ComboWordHits::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_ScoreSystem::ObjID, CUi_ScoreSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_BeatMarker::ObjID, CUi_BeatMarker::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_BeatMarkerSystem::ObjID, CUi_BeatMarkerSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_BeatJudge::ObjID, CUi_BeatJudge::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_FinalResult::ObjID, CUi_FinalResult::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_FinalResultSystem::ObjID, CUi_FinalResultSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageResultSystem::ObjID, CUi_StageResultSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageCorus::ObjID, CUi_StageCorus::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageResult_Bonus::ObjID, CUi_StageResult_Bonus::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageResult_FinalScore::ObjID, CUi_StageResult_FinalScore::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreSystem::ObjID, CUi_StoreSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_UpRightBack::ObjID, CUi_UpRightBack::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreEntry::ObjID, CUi_StoreEntry::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreAttack::ObjID, CUi_StoreAttack::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreSkill::ObjID, CUi_StoreSkill::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreItem::ObjID, CUi_StoreItem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreChip::ObjID, CUi_StoreChip::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreBar::ObjID, CUi_StoreBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreDesc::ObjID, CUi_StoreDesc::Create(m_pDevice, m_pContext))))
		assert(false);
#pragma endregion JEONGRAE

#pragma region MINA
	if (FAILED(m_pGameInstance->Add_Prototype(CMapObject_Static::ObjID, CMapObject_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMapObject_Anim::ObjID, CMapObject_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPartDoor::ObjID, CPartDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPartDoorFrame::ObjID, CPartDoorFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CProdDeptDoor::ObjID, CProdDeptDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTrigger::ObjID, CTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion MINA

#pragma region JAESEOK
	if (FAILED(m_pGameInstance->Add_Prototype(CFree_SK_Object::ObjID, CFree_SK_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Saber::ObjID, CMonster_Saber::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSaber_Weapon::ObjID, CSaber_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Gunner::ObjID, CMonster_Gunner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Baboo::ObjID, CMonster_Baboo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Samurai::ObjID, CMonster_Samurai::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion JAESEOK

#pragma region SANGHYUN
	if (FAILED(m_pGameInstance->Add_Prototype(CChai::ObjID, CChai::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBasic_Guitar::ObjID, CBasic_Guitar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion SANGHYUN

#pragma region MINHYUK

#pragma endregion MINHYUK

#pragma region Effect

	if (FAILED(m_pGameInstance->Add_Prototype(CBigSlash_Effect::ObjID, CBigSlash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CEnemySlash_Effect::ObjID, CEnemySlash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CFlare_Effect::ObjID, CFlare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Ring_Effect::ObjID, CImpact_Ring_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Shock_Effect::ObjID, CImpact_Shock_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Spark_Effect::ObjID, CImpact_Spark_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CTestEffect::ObjID, CTestEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CTexture_Effect::ObjID, CTexture_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CBeam_Effect::ObjID, CBeam_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CSnap_Impact_Effect::ObjID, CSnap_Impact_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSnap_Font_Effect::ObjID, CSnap_Font_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSmoke_Effect::ObjID, CSmoke_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CConvexLens_Effect::ObjID, CConvexLens_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CStarSymbol_Effect::ObjID, CStarSymbol_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CZap_Effect::ObjID, CZap_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CExplosion_Effect::ObjID, CExplosion_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBoom_Font_Effect::ObjID, CBoom_Font_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHarmonic_Beam::ObjID, CHarmonic_Beam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLight_Effect::ObjID, CLight_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//

	if (FAILED(m_pGameInstance->Add_Prototype(CLandImpact_Effect::ObjID, CLandImpact_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBarrier::ObjID, CBarrier::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFire_Effect::ObjID, CFire_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Effect::ObjID, CParticle_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	if (IMGUIMANAGER != nullptr) IMGUIMANAGER->Init_Effect_Prototype();
#pragma endregion Effect
#pragma endregion
	if (FAILED(m_pGameInstance->Add_Prototype(CFloor::ObjID, CFloor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFreeDynamicObject::ObjID, CFreeDynamicObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCSG_Tester::ObjID, CCSG_Tester::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CProjectile::ObjID, CProjectile::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPxProjectile::ObjID, CPxProjectile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CPreBakedBrokenObj::ObjID, CPreBakedBrokenObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCuttable::ObjID, CCuttable::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCut_Plane::ObjID, CCut_Plane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWoundable::ObjID, CWoundable::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CStaticCutTester::ObjID, CStaticCutTester::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CStaticCutPlane::ObjID, CStaticCutPlane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMyParticle::ObjID, CMyParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDynamicFracture::ObjID, CDynamicFracture::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBreakable::ObjID, CBreakable::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDamageApplier::ObjID, CDamageApplier::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSentientGopro::ObjID, CSentientGopro::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGeometryBrush::ObjID, CGeometryBrush::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

shared_ptr<CMainApp> CMainApp::Create()
{
	MAKE_SHARED_ENABLER(CMainApp);
	shared_ptr<CMainApp> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		assert(false);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	m_pGameManager->Destroy_Instance();
	m_pGameManager = nullptr;

	m_pPlayerManager->Destroy_Instance();
	m_pPlayerManager = nullptr;

	m_pImguiManager->Destroy_Instance();
	m_pImguiManager = nullptr;

	m_pCUiMgr->Destroy_Instance();
	m_pCUiMgr = nullptr;

	m_pGameInstance->Destroy_Instance();
	m_pGameInstance = nullptr;
	m_pBeat_Manager->Destroy_Instance();
	m_pBeat_Manager = nullptr;

	m_pCombat_Manager->Destroy_Instance();
	m_pCombat_Manager = nullptr;

	m_pEffect_Manager->Destroy_Instance();
	m_pEffect_Manager = nullptr;
}

