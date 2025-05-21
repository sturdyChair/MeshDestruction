#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Load_Include.h"
#include "GameManager.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Logo::~CLevel_Logo()
{
	Free();
}

HRESULT CLevel_Logo::Initialize()
{
	m_iLevelID = LEVEL_LOGO;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	{
		if(FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TEST))))
			return;
		/*if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;*/
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_DELETE))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ANIMTOOL))))
			return;
		/*if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;*/
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_F1))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_EFFECTTOOL))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_BACKSPACE))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MAPTOOL))))
			return;
		/*if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;*/
	}
	/*if (m_pGameInstance->GetKeyDown(eKeyCode::Enter))
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL))))
			return;
	}*/
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("Load"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_LOGO, strLayerTag, CBackGround::ObjID)))
		return E_FAIL;*/

	if(nullptr == m_pGameInstance.lock()->Push_Object_From_Pool(CBackGround::ObjID, LEVEL_LOGO, strLayerTag, nullptr, nullptr))
		assert(false);

	return S_OK;
}


shared_ptr<CLevel_Logo> CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_Logo
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Logo(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Logo> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{

}
