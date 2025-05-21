#include "CUi_Store.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "CUi_Default.h"
#include "CUi_Mgr.h"
#include "CUi_StoreBar.h"
#include "iostream"


wstring CUi_Store::ObjID = L"CUi_Store";

CUi_Store::CUi_Store(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_Store::CUi_Store(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_Store::~CUi_Store()
{
}

HRESULT CUi_Store::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_StoreBar::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_StoreDesc::ObjID);


	return S_OK;
}

HRESULT CUi_Store::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_StoreDesc();
	return S_OK;
}

void CUi_Store::PriorityTick(_float fTimeDelta)
{
}

void CUi_Store::Tick(_float fTimeDelta)
{

}

void CUi_Store::LateTick(_float fTimeDelta)
{

}

void CUi_Store::Progress_Store(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_UP))
	{
		if (m_iFocusIndex != 0)
		{
			m_StoreBarVec[m_iFocusIndex]->Set_Focus(false);
			--m_iFocusIndex;
			m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_DOWN))
	{
		if (m_iFocusIndex != m_StoreBarVec.size() - 1)
		{
			m_StoreBarVec[m_iFocusIndex]->Set_Focus(false);

			++m_iFocusIndex;

			m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_ESCAPE))
	{
		Return_Menu();
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
	{
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_SPACE))
	{
		m_fBuyTime += fTimeDelta;
		if (m_fBuyTime > 1)
		{
			m_fBuyTime = 0;
			Select_Bar();
		}
	}
	else
	{
		m_fBuyTime = 0;
	}
}

void CUi_Store::Adjust_Alpha(_float fTimeDelta)
{
	m_fAdjustAlpha += fTimeDelta;
	if (m_fAdjustAlpha < 0.5f)
	{
		for (auto& iter : m_StoreBarVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 4);

		for (auto& iter : m_BackVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 4);

		m_StoreDesc->Add_Color(ColorType::A, fTimeDelta * 4);
	}
}

void CUi_Store::Select_Bar()
{
	if (m_StoreBarVec[m_iFocusIndex]->Get_Price() <= PLAYERMANAGER->Get_PlayerGear())
	{
		PLAYERMANAGER->Sub_PlayerGear(m_StoreBarVec[m_iFocusIndex]->Get_Price());
		cout << "Buy" << endl;
		cout << m_StoreBarVec[m_iFocusIndex]->Get_Price() << endl;
		wcout << m_StoreBarVec[m_iFocusIndex]->Get_SkillName() << endl;

		PLAYERMANAGER->Add_Skill(m_StoreBarVec[m_iFocusIndex]->Get_SkillName());
	}
	
}

void CUi_Store::Return_Menu()
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_ESCAPE))
	{
		Set_Active(false);
		CUi_Mgr::Get_Instance()->Set_Active(L"CUi_StoreSystem", true);
	}
}


void CUi_Store::Set_Active(_bool Active)
{
	m_bActive = Active;
	for (auto& iter : m_StoreBarVec)
		iter->Set_Active(Active);

	for (auto& iter : m_BackVec)
		iter->Set_Active(Active);

	m_StoreDesc->Set_Active(Active);

	if (m_bActive)
	{
		for (auto& iter : m_StoreBarVec)
			iter->Set_Color(ColorType::A, 0);

		for (auto& iter : m_BackVec)
			iter->Set_Color(ColorType::A, 0);

		m_StoreDesc->Set_Color(ColorType::A, 0);
		m_fAdjustAlpha = 0;
	}
}

void CUi_Store::Dying()
{
	for (auto& iter : m_StoreBarVec)
		iter->Dead();

	for (auto& iter : m_BackVec)
		iter->Dead();

	m_StoreDesc->Dead();

}


void CUi_Store::Initialize_StoreDesc()
{
	CUi_StoreDesc::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = g_iWinSizeX * 0.47f;
	Desc.fSizeY = g_iWinSizeY * 0.8f;
	Desc.fX = g_iWinSizeX * 0.5f - Desc.fSizeX * 0.5f - 20;
	Desc.fY = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreDesc";
	Desc.TextureLevel = LEVEL_STATIC;
	m_StoreDesc = static_pointer_cast<CUi_StoreDesc>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreDesc::ObjID, &Desc));
}

void CUi_Store::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		break;
	default:
		assert(false);
		break;
	}

	for (auto& iter : m_StoreBarVec)
		iter->Add_Color(Type, Color);

	for (auto& iter : m_BackVec)
		iter->Add_Color(Type, Color);

	m_StoreDesc->Add_Color(Type, Color);
}

void CUi_Store::Set_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x = Color;
		break;
	case ColorType::G:
		m_fColor.y = Color;
		break;
	case ColorType::B:
		m_fColor.z = Color;
		break;
	case ColorType::A:
		m_fColor.w = Color;
		break;
	default:
		assert(false);
		break;
	}

	for (auto& iter : m_StoreBarVec)
		iter->Set_Color(Type, Color);

	for (auto& iter : m_BackVec)
		iter->Set_Color(Type, Color);

	m_StoreDesc->Set_Color(Type, Color);
}

shared_ptr<CUi_Store> CUi_Store::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_Store
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_Store(pDevice, pContext) { }
	};

	shared_ptr<CUi_Store> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_Store::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_Store
	{
		MakeSharedEnabler(const CUi_Store& rhs) : CUi_Store(rhs) { }
	};

	shared_ptr<CUi_Store> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
