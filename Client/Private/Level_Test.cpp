#include "Level_Test.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"
#include "SkyBox.h"

#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "Beat_Manager.h"


CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Test::~CLevel_Test()
{
	Free();
}

HRESULT CLevel_Test::Initialize()
{
	m_iLevelID = LEVEL_TEST;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);
	//CImgui_Manager::Get_Instance()->Set_ToolType(eToolType::TOOL_MAP, false);
	/*Player */
	//CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;
	Ready_Layer_Object(TEXT("Layer_Object"));

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	if(FAILED(Ready_Layer_Bgm()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	//Ready_Ui();

	return S_OK;
}

void CLevel_Test::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	//ShowCursor(false);

	/*if (GAMEINSTANCE->Get_KeyDown(DIK_1))
		Level_Chanege_Sequenece(fTimeDelta);*/

	//if (GAMEINSTANCE->Get_KeyDown(DIK_SPACE))
	//{
	//	static _float4 pos[6]{ {20.f, 0.f, 0.f, 1.f}, {-20.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 20.f, 1.f}, {0.f, 0.f, -20.f, 1.f},
	//							{0.f, 10.f, 0.f, 1.f}, {0.f, -10.f, 0.f, 1.f}};
	//							
	//	static _float3 dir[6]{ {-1.f,0.f,0.f}, {1.f,0.f,0.f}, {0.f,0.f,-1.f}, {0.f,0.f,1.f},
	//							{ 0.f,-1.f,0.f}, { 0.f,1.f,0.f} };
	//	static _uint projIdx = 0;
	//	_vector vLook = XMLoadFloat3(&dir[projIdx]);
	//	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
	//		return;
	//	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	//	_vector vUp = XMVector3Cross(vLook, vRight);
	//	_vector vTrans = XMLoadFloat4(&pos[projIdx]);
	//	_float4x4 matrix;

	//	XMStoreFloat4x4(&matrix, _matrix{ vRight, vUp, vLook, vTrans });
	//	GAMEINSTANCE->Push_Object_From_Pool(CProjectile::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"Layer_Projectile",
	//		&matrix);
	//	++projIdx;
	//	projIdx %= 6;
	//}

	if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
	{
		GAMEINSTANCE->Push_Object_From_Pool(CProjectile::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"Layer_Projectile",
			GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix_Ptr());
	}
	static _float fPower = 1.f;
	if (GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
	{
		fPower = 1.f;
	}
	if (GAMEINSTANCE->Get_MousePressing(DIMK_RBUTTON))
	{
		fPower += fTimeDelta;
	}

	if (GAMEINSTANCE->Get_MouseUp(DIMK_RBUTTON))
	{
		_matrix World = GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix_XMMat();
		World.r[2] *= min(fPower, 10.f);
		_float4x4 matrix;
		XMStoreFloat4x4(&matrix, World);
		GAMEINSTANCE->Push_Object_From_Pool(CPxProjectile::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"Layer_Projectile",
			&matrix);
	}

	return;
}

HRESULT CLevel_Test::Render()
{
	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CSentientGopro::GOPRO_DESC			CameraDesc{};
	
	CameraDesc.fMouseSensor = 0.075f;
	CameraDesc.vEye = _float4(0.f, 5.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.strTag = TEXT("Camera_Main");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	
	shared_ptr<CSentientGopro> Camera = static_pointer_cast<CSentientGopro>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, CSentientGopro::ObjID, &CameraDesc));

	//CCamera_Tool::CAMERA_FREE_DESC			CameraDesc{};
	//
	//CameraDesc.fMouseSensor = 0.075f;
	//CameraDesc.vEye = _float4(0.f, 5.f, -5.f, 1.f);
	//CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//CameraDesc.fFovy = XMConvertToRadians(90.0f);
	//CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	//CameraDesc.fNear = 0.1f;
	//CameraDesc.fFar = 3000.f;
	//CameraDesc.fSpeedPerSec = 10.f;
	//CameraDesc.strTag = TEXT("Camera_Main");
	//CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	//
	//shared_ptr<CCamera_Tool> Camera = static_pointer_cast<CCamera_Tool>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));

	m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Main"), Camera);
	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Main"));

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Map(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_SkyBox"))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(0.f, -5.0f, 0.f, 1.f);
	TransformDesc.vScale = _float3(10000.f, 10.0f, 10000.f);
	/* Monsters */
	GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CFloor::ObjID, &TransformDesc);

	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_GridBox"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Monster(const wstring& strLayerTag)
{

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	/* Monsters */
	GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Saber::ObjID, &TransformDesc);

	TransformDesc.vPosition = _float4(0.f, 0.f, 10.f, 1.f);
	GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Baboo::ObjID, &TransformDesc);



	//GAMEINSTANCE->Map_Load(LEVEL_FIRSTSTAGE);
	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Bgm()
{

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Effect(const wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Object(const wstring& strLayerTag)
{
	//{
	//	CFreeDynamicObject::FDO_DESC Desc{};
	//	Desc.vPosition = _float4(0.f, 5.f, 0.f, 1.f);
	//	Desc.strModelTag = L"Prototype_Component_Model_Cube";
	//	Desc.vScale = { 50.f,50.f,50.f };

	//	GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CFreeDynamicObject::ObjID, &Desc);

	//	Desc.vPosition = _float4(5.f, 5.f, 0.f, 1.f);
	//	Desc.strModelTag = L"Prototype_Component_Model_Sphere";
	//	Desc.vScale = { 10.f,10.f,10.f };
	//	GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CFreeDynamicObject::ObjID, &Desc);
	//}

	//{
	//	CCSG_Tester::FDO_DESC CSGDesc{};
	//	CSGDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	//	CSGDesc.strModelTag = L"Prototype_Component_Model_Torus";
	//	CSGDesc.vScale = { 100.f,100.f,100.f };
	//
	//	GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CCSG_Tester::ObjID, &CSGDesc);
	//}

	//{
	//	CBreakable::FDO_DESC CSGDesc{};
	//	CSGDesc.vPosition = _float4(0.f, -2.f, 0.f, 1.f);
	//	CSGDesc.strModelTag = L"Prototype_Component_Model_Cube";
	//	CSGDesc.vScale = { 400.f,200.f,200.f };
	//	CSGDesc.iNumCell = 500;
	//	CSGDesc.fTorqueThreshold = 50.f;
	//	CSGDesc.fForceThreshold = 50.f;
	//	GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CBreakable::ObjID, &CSGDesc);
	//}
	
	//{
	//	GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CDamageApplier::ObjID);
	//}


	//{
	//	CPreBakedBrokenObj::FDO_DESC Desc{};
	//	Desc.vPosition = _float4(10.f, 0.f, 0.f, 1.f);
	//	Desc.strModelTag = L"Prototype_Component_Model_Torus_0";
	//	Desc.vScale = { 100.f,100.f,100.f };
	//	GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CPreBakedBrokenObj::ObjID, &Desc);
	//
	//}

#pragma region NATHANS
	
	{
		CProjectile::FDO_DESC Desc{};
		Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		Desc.strModelTag = L"Prototype_Component_Model_Sphere";
		Desc.vScale = { 20.f,20.f,20.f };
		Desc.fSpeedPerSec = 50.f;
		for (_uint i = 0; i < 30; ++i)
			GAMEINSTANCE->Pool_Object(CProjectile::ObjID, &Desc);
		for (_uint i = 0; i < 64; ++i)
			GAMEINSTANCE->Pool_Object(CMyParticle::ObjID);

		CPxProjectile::FDO_DESC Desc2{};
		Desc2.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		Desc2.strModelTag = L"Prototype_Component_Model_Sphere";
		Desc2.vScale = { 50.f,50.f,50.f };
		Desc2.fSpeedPerSec = 50.f;
		for (_uint i = 0; i < 30; ++i)
			GAMEINSTANCE->Pool_Object(CPxProjectile::ObjID, &Desc2);
	}
	{
		CCuttable::FDO_DESC Desc{};
		Desc.vPosition = _float4(0.f, -4.f, 0.f, 1.f);
		Desc.strModelTag = L"Prototype_Component_Model_Nathan";
		Desc.vScale = { 5.f,5.f,5.f };
		auto pCA = GAMEINSTANCE->Add_Clone_Return(m_iLevelID, strLayerTag, CCuttable::ObjID, &Desc);
		GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CCut_Plane::ObjID, &pCA);
	}
	
	{ //SM cutter
		CStaticCutTester::FDO_DESC Desc{};
		Desc.vPosition = _float4(15.f, -4.f, 0.f, 1.f);
		Desc.strModelTag = L"Prototype_Component_Model_Nathan_Mani";
		Desc.vScale = { 5.f,5.f,5.f };
		auto pCA = GAMEINSTANCE->Add_Clone_Return(m_iLevelID, strLayerTag, CStaticCutTester::ObjID, &Desc);
		GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CStaticCutPlane::ObjID, &pCA);
	}

	//{ 
	//	CDynamicFracture::FDO_DESC Desc{};
	//	Desc.vPosition = _float4(20.f, 0.f, 0.f, 1.f);
	//	Desc.strModelTag = L"Prototype_Component_Model_Nathan_Mani_Mani";
	//	//
	//	Desc.vScale = { 5.f,5.f,5.f };
	//	Desc.iNumCell = 50;
	//	auto pDF =static_pointer_cast<CDynamicFracture>( GAMEINSTANCE->Add_Clone_Return(m_iLevelID, strLayerTag, CDynamicFracture::ObjID, &Desc));
	//	auto pDF2 = pDF->ReClone();
	//	pDF2->Set_Position({25.f,0.f,0.f});
	//	GAMEINSTANCE->Push_Object(m_iLevelID, strLayerTag, pDF2);
	//
	//	//GAMEINSTANCE->Add_Clone(m_iLevelID, strLayerTag, CDynamicFracture::ObjID, &Desc);
	//	
	//}

	{
		CWoundable::FDO_DESC Desc{};
		Desc.vPosition = _float4(30.f, -4.f, 0.f, 1.f);
		Desc.strModelTag = L"Prototype_Component_Model_Nathan";
		Desc.vScale = { 5.f,5.f,5.f };
		auto pCA = GAMEINSTANCE->Add_Clone_Return(m_iLevelID, strLayerTag, CWoundable::ObjID, &Desc);
	}
#pragma endregion
	return S_OK;
}

HRESULT CLevel_Test::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.g_fIntensity = 1.f;

	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	return S_OK;

}

void CLevel_Test::Ready_Ui()
{
	CBeat_Manager::Get_Instance()->Set_Beat(0.4f);
	CUi_Mgr::Get_Instance()->Add_Clone(CUi_BeatMarkerSystem::ObjID, LEVEL_TEST, L"CUi", CUi_BeatMarkerSystem::ObjID, nullptr);
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_RankSystem::ObjID, LEVEL_TEST, L"CUi", CUi_RankSystem::ObjID, nullptr);
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_StageResultSystem::ObjID, LEVEL_TEST, L"CUi", CUi_StageResultSystem::ObjID);
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_ScoreSystem::ObjID, LEVEL_TEST, L"CUi", CUi_ScoreSystem::ObjID, nullptr);
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_StoreSystem::ObjID, LEVEL_TEST, L"CUi", CUi_StoreSystem::ObjID, nullptr);
}

void CLevel_Test::Tester()
{

}

void CLevel_Test::Level_Chanege_Sequenece(_float fTimeDelta)
{
	GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_Test> CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_Test
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Test(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Test> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Test"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Test::Free()
{

}
