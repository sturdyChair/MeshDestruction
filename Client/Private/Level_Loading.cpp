#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"

#include "Level_Logo.h"
#include "Level_Test.h"
#include "Level_AnimTool.h"
#include "Level_MapTool.h"
#include "Level_Effect.h"
#include "GameManager.h"

#include "Load_Include.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Loading::~CLevel_Loading()
{
	Free();
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_iLevelID = LEVEL_LOADING;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	m_eNextLevelID = eNextLevelID;

	//if (FAILED(Ready_Ui(L"Layer_Ui")))
	//	return E_FAIL;

	/* 자원로딩을 위한 로더객체를 생성해준다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	
	if (nullptr == m_pLoader)
		return E_FAIL;

	/*GAMEMANAGER->VideoSetting();*/
	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			shared_ptr<CLevel> pLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_TEST:
				pLevel = CLevel_Test::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_ANIMTOOL:
				pLevel = CLevel_AnimTool::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_MAPTOOL:
				pLevel = CLevel_MapTool::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_EFFECTTOOL:
				pLevel = CLevel_Effect::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;

			if (FAILED(m_pGameInstance.lock()->Change_Level(pLevel)))
				return;
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Show_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Ui(const wstring& strLayerTag)
{
#pragma region CUi_LoadingCat
	CUi_2D::Desc    CatDesc{};

	CatDesc.fX = -420.f;
	CatDesc.fY = -160.f;
	CatDesc.fZ = 0.7f;
	CatDesc.fSizeX = 342;
	CatDesc.fSizeY = 346;
	CatDesc.fSizeZ = 1.f;
	CatDesc.fSpeed = 0.f;
	CatDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	CatDesc.TextureFileName = L"Prototype_Component_Texture_LoadingCat";
	CatDesc.TextureLevel = LEVEL_STATIC;

	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_LOADING, strLayerTag, CUi_LoadingCat::ObjID, &CatDesc)))
		assert(false);

#pragma endregion CUi_LoadingCat

#pragma region CUi_LoadingNote

	CUi_LoadingNote::Desc Note1Desc{};
	Note1Desc.fX = -300.f;
	Note1Desc.fY = -125.f;
	Note1Desc.fZ = 0.7f;
	Note1Desc.fSizeX = 80.f;
	Note1Desc.fSizeY = 110.f;
	Note1Desc.fSizeZ = 1.f;
	Note1Desc.fSpeed = 0.f;
	Note1Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Note1Desc.TextureFileName = L"Prototype_Component_Texture_LoadingNote1";
	Note1Desc.TextureLevel = LEVEL_STATIC;
	Note1Desc.fAppearTime = 0.2f;
	Note1Desc.fDisappearTime = 2.f;

	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_LOADING, strLayerTag, CUi_LoadingNote::ObjID, &Note1Desc)))
	    assert(false);

	CUi_LoadingNote::Desc Note2Desc = Note1Desc;
	Note2Desc.fX = -210.f;
	Note2Desc.fY = -125.f;
	Note2Desc.TextureFileName = L"Prototype_Component_Texture_LoadingNote2";
	Note2Desc.fAppearTime = 1.2f;
	Note2Desc.fDisappearTime = 2.f;

	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_LOADING, strLayerTag, CUi_LoadingNote::ObjID, &Note2Desc)))
		assert(false);

	CUi_LoadingNote::Desc Note3Desc = Note2Desc;
	Note3Desc.fX = -255.f;
	Note3Desc.fY = -105.f;
	Note3Desc.TextureFileName = L"Prototype_Component_Texture_LoadingNote3";
	Note3Desc.fAppearTime = 0.8f;
	Note3Desc.fDisappearTime = 2.f;

	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_LOADING, strLayerTag, CUi_LoadingNote::ObjID, &Note3Desc)))
		assert(false);

	CUi_LoadingNote::Desc Note4Desc = Note3Desc;
	Note4Desc.fX = -155.f;
	Note4Desc.fY = -95.f;
	Note4Desc.TextureFileName = L"Prototype_Component_Texture_LoadingNote1";
	Note4Desc.fAppearTime = 1.4f;
	Note4Desc.fDisappearTime = 2.f;

	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_LOADING, strLayerTag, CUi_LoadingNote::ObjID, &Note4Desc)))
		assert(false);

#pragma endregion CUi_LoadingNote

	return S_OK;
}

shared_ptr<CLevel_Loading> CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	struct MakeSharedEnabler : public CLevel_Loading
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Loading(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Loading> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{

}
