#include "CUi_StoreSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "CUi_StoreEntry.h"
#include "CUi_Mgr.h"
#include "iostream"
#include "CUi_UpRightBack.h"
#include "CUi_StoreAttack.h"
#include "CUi_StoreChip.h"
#include "CUi_StoreChipEquip.h"
#include "CUi_StoreItem.h"
#include "CUi_StoreItemEquip.h"
#include "CUi_StoreSkill.h"
#include "CUi_StoreBar.h"


wstring CUi_StoreSystem::ObjID = L"CUi_StoreSystem";

CUi_StoreSystem::CUi_StoreSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StoreSystem::CUi_StoreSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StoreSystem::~CUi_StoreSystem()
{
}

HRESULT CUi_StoreSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.emplace_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.emplace_back(CUi_UpRightBack::ObjID);
	m_List_Owning_ObjectID.emplace_back(CUi_StoreEntry::ObjID);


	m_List_Owning_ObjectID.emplace_back(CUi_StoreAttack::ObjID);
	m_List_Owning_ObjectID.emplace_back(CUi_StoreSkill::ObjID);
	m_List_Owning_ObjectID.emplace_back(CUi_StoreItem::ObjID);
	m_List_Owning_ObjectID.emplace_back(CUi_StoreChip::ObjID);

	m_List_Owning_ObjectID.emplace_back(CUi_StoreDesc::ObjID);
	m_List_Owning_ObjectID.emplace_back(CUi_StoreBar::ObjID);


	return S_OK;
}

HRESULT CUi_StoreSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_Menu();
	Initialize_BackGround();
	Initialize_Opening();
	Initialize_StoreDefaultBack();
	m_MenuVec[m_iFocusIndex]->Set_TextureNum(1);

	return S_OK;
}

void CUi_StoreSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreSystem::Tick(_float fTimeDelta)
{
	m_KeyOverlapPreventTime += fTimeDelta;
	if (m_bActive)
	{
		Opening(fTimeDelta);
		Alpha_Adjust(fTimeDelta);
		Check_Menu();
		Select_Menu();
	}
}

void CUi_StoreSystem::LateTick(_float fTimeDelta)
{
	
}

void CUi_StoreSystem::Set_Active(_bool Active)
{
	m_KeyOverlapPreventTime = 0;
	m_bActive = Active;
	if (m_bActive == false)
	{
		for (auto& iter : m_MenuVec)
			iter->Set_Active(false);

		for (auto& iter : m_BackGroundVec)
			iter->Set_Active(false);
	}
	else
	{
		for (auto& iter : m_MenuVec)
		{
			iter->Set_Active(true);
			iter->Set_Color(ColorType::A, 0);
		}

		for (auto& iter : m_BackGroundVec)
		{
			iter->Set_Active(true);
			iter->Set_Color(ColorType::A, 0);
		}
		m_fAlphaTime = 0;
	}
}

void CUi_StoreSystem::Initialize_Menu()
{
	m_MenuVec.reserve(4);

	FontDescInfo FontDesc;
	FontDesc.DirectPos = { -110, -105 };
	FontDesc.String = L"공격";
	FontDesc.Size = 1.f;


	CUi_StoreEntry::Desc Desc;
	Desc.fSizeX = 350;
	Desc.fSizeY = 330;
	Desc.fX = -240 * 1.5f + 30;
	Desc.fY = 30;
	Desc.fSpeed = 300;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreMenuAttack";
	Desc.TextureLevel = LEVEL_STATIC;

	Desc.Font = FontDesc;
	m_MenuVec.emplace_back(static_pointer_cast<CUi_StoreEntry>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreEntry::ObjID, &Desc)));


	Desc.fX = -240 * 0.5f + 30;
	FontDesc.String = L"특수 공격";
	FontDesc.DirectPos = { -150, -105 };
	Desc.Font = FontDesc;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreMenuSkill";
	m_MenuVec.emplace_back(static_pointer_cast<CUi_StoreEntry>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreEntry::ObjID, &Desc)));

	Desc.fX = 240 * 0.5f + 30;
	FontDesc.String = L"아이템";
	FontDesc.DirectPos = { -125, -105 };
	Desc.Font = FontDesc;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreMenuItem";
	m_MenuVec.emplace_back(static_pointer_cast<CUi_StoreEntry>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreEntry::ObjID, &Desc)));

	Desc.fX = 240 * 1.5f + 30;
	FontDesc.String = L"칩";
	FontDesc.DirectPos = { -82, -105 };
	Desc.Font = FontDesc;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreMenuCustom";
	m_MenuVec.emplace_back(static_pointer_cast<CUi_StoreEntry>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreEntry::ObjID, &Desc)));

	m_MenuVec[0]->Set_Focus(true);
}

void CUi_StoreSystem::Initialize_BackGround()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = Desc.fSizeX * 0.05f;
	Desc.fX = 0;
	Desc.fY = g_iWinSizeY * 0.5f - Desc.fSizeY * 0.5f;
	Desc.fZ = 0.95f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBackUp";
	Desc.TextureLevel = LEVEL_STATIC;
	m_BackGroundVec.emplace_back(static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fX = 0;
	Desc.fY = -_float(g_iWinSizeY * 0.5f) + Desc.fSizeY * 0.5f;
	Desc.fZ = 0.95f;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBackDown";
	m_BackGroundVec.emplace_back(static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));


	Desc.fSizeX = 220;
	Desc.fSizeY = 60;
	Desc.fX = -_float(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.5f + 30;
	Desc.fY = g_iWinSizeY * 0.5f - Desc.fSizeY * 0.5f - 20;
	Desc.fZ = 0.9f;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreTitle";
	m_BackGroundVec.emplace_back(static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fSizeX = 40;
	Desc.fSizeY = 40;
	Desc.fX = -_float(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.5f + 30;
	Desc.fY = g_iWinSizeY * 0.5f - Desc.fSizeY * 0.5f - 20;
	Desc.fZ = 0.9f;
	Desc.TextureFileName = L"Prototype_Component_Texture_Gear";
	m_BackGroundVec.emplace_back(static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
}

void CUi_StoreSystem::Initialize_Opening()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = g_iWinSizeX + 300;
	Desc.fSizeY = g_iWinSizeY * 0.5f;
	Desc.fX = 0;
	Desc.fY = g_iWinSizeY * 0.5f - Desc.fSizeY * 0.5f;
	Desc.fZ = 0.1f;
	Desc.fSpeed = 6500;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreOpen";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pOpeningVec.emplace_back(static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fY = -Desc.fSizeY * 0.5f;
	m_pOpeningVec.emplace_back(static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	m_pOpeningVec[1]->Set_TextureNum(1);
}

void CUi_StoreSystem::Initialize_StoreDefaultBack()
{
	CUi_UpRightBack::Desc Desc;
	Desc.fSizeX = g_iWinSizeX * 0.3f;
	Desc.fSizeY = 60;
	Desc.fX = g_iWinSizeX * 0.5f - Desc.fSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f - Desc.fSizeY * 0.5f;
	Desc.fZ = 0.8f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBackUpRight";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.fColor = { 1, 1, 1, 0 };
	CUi_Mgr::Get_Instance()->Add_Clone(L"CUi_UpRightBack", 
		m_iLevelIndex, L"CUi", CUi_UpRightBack::ObjID, &Desc);
}

void CUi_StoreSystem::Check_Menu()
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_LEFT))
	{
		if (m_iFocusIndex > 0)
		{
			--m_iFocusIndex;
			for (auto& iter : m_MenuVec)
				iter->Set_Focus(false);
			m_MenuVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_RIGHT))
	{
		if (m_iFocusIndex < m_MenuVec.size() - 1)
		{
			++m_iFocusIndex;
			for (auto& iter : m_MenuVec)
				iter->Set_Focus(false);
			m_MenuVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
}

void CUi_StoreSystem::Select_Menu()
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	{
		CUi_Store::Desc Desc;
		Set_Active(false);
		switch (m_iFocusIndex)
		{
		case 0:	
			CUi_Mgr::Get_Instance()->Add_Clone(L"CUi_StoreAttack",
				m_iLevelIndex, L"CUi", CUi_StoreAttack::ObjID, &Desc);
			break;
		case 1:
			CUi_Mgr::Get_Instance()->Add_Clone(L"CUi_StoreSkill",
				m_iLevelIndex, L"CUi", CUi_StoreSkill::ObjID, &Desc);
			break;
		case 2:
			CUi_Mgr::Get_Instance()->Add_Clone(L"CUi_StoreItem",
				m_iLevelIndex, L"CUi", CUi_StoreItem::ObjID, &Desc);
			break;
		case 3:
			CUi_Mgr::Get_Instance()->Add_Clone(L"CUi_StoreChip",
				m_iLevelIndex, L"CUi", CUi_StoreChip::ObjID, &Desc);
			break;
		default:
			assert(false);
			break;
		}
	}

	if (m_pGameInstance.lock()->Get_KeyDown(DIK_ESCAPE))
	{
		if (m_KeyOverlapPreventTime > 0.5f)
		{
			CUi_Mgr::Get_Instance()->Ui_AllDead();

			for (auto& iter : m_MenuVec)
				iter->Dead();

			for (auto& iter : m_BackGroundVec)
				iter->Dead();
		}
	}
}

void CUi_StoreSystem::Opening(_float fTimeDelta)
{ 
	m_fOpeningTime += fTimeDelta;
	if (m_fOpeningTime < 0.5f)
	{
		m_pOpeningVec[0]->Move(MoveDir::Left, fTimeDelta);
		m_pOpeningVec[1]->Move(MoveDir::Right, fTimeDelta);
	}
}

void CUi_StoreSystem::Alpha_Adjust(_float fTimeDelta)
{
	m_fAlphaTime += fTimeDelta;
	if (m_fAlphaTime < 0.5f)
	{
		for (auto& iter : m_MenuVec)
		{
			iter->Add_Color(ColorType::A, fTimeDelta * 4);
		}
		for (auto& iter : m_BackGroundVec)
		{
			iter->Add_Color(ColorType::A, fTimeDelta * 4);
		}
	}
}

shared_ptr<CUi_StoreSystem> CUi_StoreSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreSystem
	{
		MakeSharedEnabler(const CUi_StoreSystem& rhs) : CUi_StoreSystem(rhs) { }
	};

	shared_ptr<CUi_StoreSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
