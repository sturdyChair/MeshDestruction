#include "Level_Effect.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"
#include "SkyBox.h"

#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"
#include "TestEffect.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Effect::~CLevel_Effect()
{
	Free();
}

HRESULT CLevel_Effect::Initialize()
{
	m_iLevelID = LEVEL_TEST;

	IMGUIMANAGER->Set_ToolType(eToolType::TOOL_EFFECT,false);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Bgm()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;


	return S_OK;
}

void CLevel_Effect::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Effect Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	//ShowCursor(false);

	/*if (GAMEINSTANCE->Get_KeyDown(DIK_1))
		Level_Chanege_Sequenece(fTimeDelta);*/

	return;
}

HRESULT CLevel_Effect::Render()
{

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Tool::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(0.f, 0.f, -25.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 50.f;
	CameraDesc.strTag = TEXT("Camera_Tool_Main");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CCamera_Tool> Camera = static_pointer_cast<CCamera_Tool>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));

	m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Tool_Main"), Camera);
	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Map(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_SkyBox"))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CMapObject_Static::ObjID)))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_TEST, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;*/
		/*CTransform::TRANSFORM_DESC			TransformDesc{};
		TransformDesc.fSpeedPerSec = 1.f;
		TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_Map_KillRun_1"), &TransformDesc)))
			return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Monster(const wstring& strLayerTag)
{

	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CTestModel::ObjID)))
		return E_FAIL;*/

		//GAMEINSTANCE->Map_Load(LEVEL_FIRSTSTAGE);
	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Bgm()
{

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Effect(const wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_EFFECTTOOL, strLayerTag, CTexture_Effect::ObjID)))
	//	return E_FAIL;
	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_EFFECTTOOL, strLayerTag, CBigSlash_Effect::ObjID)))
		return E_FAIL;*/
	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_EFFECTTOOL, strLayerTag, CEnemySlash_Effect::ObjID)))
		return E_FAIL;*/
	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_EFFECTTOOL, strLayerTag, CFlare_Effect::ObjID)))
		return E_FAIL;*/
	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_EFFECTTOOL, strLayerTag, CImpact_Spark_Effect::ObjID)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Effect::Ready_Light()
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

void CLevel_Effect::Level_Chanege_Sequenece(_float fTimeDelta)
{
	GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_Effect> CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Effect(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Effect"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Effect::Free()
{

}
