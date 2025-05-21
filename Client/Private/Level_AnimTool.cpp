#include "Level_AnimTool.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SkyBox.h"

#include "PlayerManager.h"
#include "Load_Include.h"
#include "Imgui_Manager.h"
#include "GameManager.h"

CLevel_AnimTool::CLevel_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_AnimTool::~CLevel_AnimTool()
{
	Free();
}

HRESULT CLevel_AnimTool::Initialize()
{
	m_iLevelID = LEVEL_ANIMTOOL;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;



	return S_OK;
}

void CLevel_AnimTool::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	DebugMode();
	//ShowCursor(false);

	/*if (GAMEINSTANCE->Get_KeyDown(DIK_1))
		Level_Chanege_Sequenece(fTimeDelta);*/

	return;
}

HRESULT CLevel_AnimTool::Render()
{

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Tool::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(60.f, 20.f, -55.f, 1.f);
	CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.strTag = TEXT("Camera_Tool_Main");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CCamera_Tool> Camera = static_pointer_cast<CCamera_Tool>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));

	m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Tool_Main"), Camera);
	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_Monster(const wstring& strLayerTag)
{

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(60.f, 20.f, -40.f, 1.f);

	//GAMEINSTANCE->Map_Load(LEVEL_FIRSTSTAGE);
	return S_OK;
}


HRESULT CLevel_AnimTool::Ready_Light()
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

void CLevel_AnimTool::DebugMode()
{


}

void CLevel_AnimTool::Level_Chanege_Sequenece(_float fTimeDelta)
{
	GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_AnimTool> CLevel_AnimTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_AnimTool
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_AnimTool(pDevice, pContext) { }
	};

	shared_ptr<CLevel_AnimTool> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_AnimTool"));
		assert(false);
	}

	return pInstance;
}

void CLevel_AnimTool::Free()
{

}
