#include "CUi_ScoreSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "CUi_Default.h"


wstring CUi_ScoreSystem::ObjID = L"CUi_ScoreSystem";

CUi_ScoreSystem::CUi_ScoreSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_ScoreSystem::CUi_ScoreSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_ScoreSystem::~CUi_ScoreSystem()
{
}

HRESULT CUi_ScoreSystem::Initialize_Prototype()
{
	if (FAILED(m_pGameInstance.lock()->Add_Font
	(L"Nexon", L"../Bin/Resources/Textures/Ui/Font/Nexon.spritefont")))
		assert(false);


	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);


	return S_OK;
}

HRESULT CUi_ScoreSystem::Initialize(void* pArg)
{
	Initialize_Transform();

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	//InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);
	 
	Initialize_ScoreBack();

	return S_OK;
}

void CUi_ScoreSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_ScoreSystem::Tick(_float fTimeDelta)
{
	Check_PlayerScore();

	if (m_bChange)
	{
		StringMove(fTimeDelta);

	}
}

void CUi_ScoreSystem::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_ScoreSystem::Render()
{
	RenderFont(m_FontDesc);

	return S_OK;
}

_float2 CUi_ScoreSystem::Change_APICoordinate(_float2 DirectPos)
{
	_float2 APIPos = { 0, 0 };
	APIPos.x = DirectPos.x + g_iWinSizeX * 0.5f;
	APIPos.y = -DirectPos.y + g_iWinSizeY * 0.5f;
	return APIPos;
}

void CUi_ScoreSystem::RenderFont(FontDescInfo Desc, _uint EnterGap, _uint EnterCount)
{
	m_pGameInstance.lock()->Render_Font(Desc.FontTag, Desc.String,
		Change_APICoordinate(Desc.DirectPos), XMLoadFloat4(&Desc.Color), Desc.Rotation, Desc.Size);

	m_pGameInstance.lock()->Render_Font(Desc.FontTag, L"Á¡¼ö",
		Change_APICoordinate(_float2(m_fOriginPos.x - 90, m_fOriginPos.y + 40)), XMVectorSet(1, 1, 0, 1), Desc.Rotation, 0.3f);
}

void CUi_ScoreSystem::Check_PlayerScore()
{
	_uint Score = PLAYERMANAGER->Get_PlayerScore();
	if (m_iPrevScore != Score)
	{
		m_iPrevScore = Score;
		m_bChange = true;
		m_FontDesc.String = to_wstring(Score);
		m_FontDesc.DirectPos.x = m_fOriginPos.x;
		m_FontDesc.DirectPos.x -= m_FontDesc.String.size() * 15;
	}
}

void CUi_ScoreSystem::StringMove(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	if (m_fMoveTime <= 0.04f)
	{
		m_FontDesc.DirectPos.y += m_fSpeed * fTimeDelta;
	}
	else if (0.04f < m_fMoveTime && m_fMoveTime < 0.08f)
	{
		m_FontDesc.DirectPos.y -= m_fSpeed * fTimeDelta;
	}
	else
	{
		m_fMoveTime = 0;
		m_FontDesc.DirectPos.y = m_fOriginPos.y;
		m_bChange = false;
	}
}

void CUi_ScoreSystem::Initialize_ScoreBack()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 200;
	Desc.fSizeY = 25;
	Desc.fX = m_fOriginPos.x - 20;
	Desc.fY = m_fOriginPos.y + 30;
	Desc.fZ = 0.9f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ScoreBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pScoreBack = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

shared_ptr<CUi_ScoreSystem> CUi_ScoreSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_ScoreSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_ScoreSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_ScoreSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_ScoreSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_ScoreSystem
	{
		MakeSharedEnabler(const CUi_ScoreSystem& rhs) : CUi_ScoreSystem(rhs) { }
	};

	shared_ptr<CUi_ScoreSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
