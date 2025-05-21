#include "Level_MapTool.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SkyBox.h"

#include "Load_Include.h"
#include "Imgui_Manager.h"

CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_MapTool::~CLevel_MapTool()
{
	Free();
}

HRESULT CLevel_MapTool::Initialize()
{
	m_iLevelID = LEVEL_MAPTOOL;

	CImgui_Manager::Get_Instance()->Set_ToolType(eToolType::TOOL_MAP, false);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
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

HRESULT CLevel_MapTool::Render()
{

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Tool::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(60.f, 20.f, -55.f, 1.f);
	CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 30.f;
	CameraDesc.strTag = TEXT("Camera_Tool_Main");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CCamera_Tool> Camera = static_pointer_cast<CCamera_Tool>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));

	m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Tool_Main"), Camera);
	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Monster(const wstring& strLayerTag)
{

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(60.f, 20.f, -40.f, 1.f);

	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CVergil::ObjID)))
	//	return E_FAIL;
	//GAMEINSTANCE->Map_Load(LEVEL_FIRSTSTAGE);

	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CTrigger::ObjID)))
	//	return E_FAIL;


	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	animDesc.fSpeedPerSec = 0.f;
	animDesc.fRotationPerSec = 0.f;
	animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
	animDesc.strModelTag = TEXT("SK_bg0140");

	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CMapObject_Anim::ObjID, &animDesc)))
	//	return E_FAIL;

	//animDesc.strModelTag = TEXT("SK_bg0140");
	//
	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CMapObject_Anim::ObjID, &animDesc)))
	//	return E_FAIL;

	if (FAILED(GAMEINSTANCE->Add_Clone(LEVEL_STATIC, strLayerTag, CProdDeptDoor::ObjID)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_MapTool::Ready_Light()
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

void CLevel_MapTool::DebugMode()
{


}

void CLevel_MapTool::Level_Chanege_Sequenece(_float fTimeDelta)
{
	GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_MapTool> CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_MapTool
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_MapTool(pDevice, pContext) { }
	};

	shared_ptr<CLevel_MapTool> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_MapTool"));
		assert(false);
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{

}
