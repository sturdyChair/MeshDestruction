#include "CUi_FinalResultSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "CUi_FinalResult.h"


wstring CUi_FinalResultSystem::ObjID = L"CUi_FinalResultSystem";

CUi_FinalResultSystem::CUi_FinalResultSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_FinalResultSystem::CUi_FinalResultSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_FinalResultSystem::~CUi_FinalResultSystem()
{
}

HRESULT CUi_FinalResultSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_FinalResult::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	return S_OK;
}

HRESULT CUi_FinalResultSystem::Initialize(void* pArg)
{
	Initialize_Transform();

	return S_OK;
}

void CUi_FinalResultSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_FinalResultSystem::Tick(_float fTimeDelta)
{
	m_fCreateTimeGap += fTimeDelta;
	if (m_fCreateTimeGap > 0.25f && m_iCount < 5)
	{
		m_fCreateTimeGap = 0;
		Create();
	}
}

void CUi_FinalResultSystem::LateTick(_float fTimeDelta)
{
	
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_FinalResultSystem::Render()
{

	return S_OK;
}

void CUi_FinalResultSystem::Create()
{
	CUi_FinalResult::Desc Desc;

	if (m_iCount == 0)
	{
		Desc.eFontType = CUi_FinalResult::FontType::Corus;
		Desc.fColor = { 1, 1, 1, 0.f };
		Desc.fSizeX = 400;
		Desc.fSizeY = 40;
		Desc.fX = 500;
		Desc.fY = 200;
		Desc.fZ = 0.7f;
		Desc.fSpeed = 50;
		Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
		Desc.ShaderPass = ShaderPass::Blend;
		Desc.TextureFileName = L"Prototype_Component_Texture_FinalResult";
		Desc.TextureLevel = LEVEL_STATIC;
	}
	else
	{
		Desc.eFontType = (CUi_FinalResult::FontType)m_iCount;
		Desc.fColor = { 1, 1, 1, 0.f };
		Desc.fSizeX = 250;
		Desc.fSizeY = 30;
		Desc.fX = 450;
		Desc.fY = 200 - m_iCount * 80.f;
		Desc.fZ = 0.7f;
		Desc.fSpeed = 50;
		Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
		Desc.ShaderPass = ShaderPass::Blend;
		Desc.TextureFileName = L"Prototype_Component_Texture_FinalResult";
		Desc.TextureLevel = LEVEL_STATIC;
	}

	m_pResultVec.emplace_back(static_pointer_cast<CUi_FinalResult>
		(m_pGameInstance.lock()->Add_Clone_Return
		(m_iLevelIndex, L"CUi", CUi_FinalResult::ObjID, &Desc)));

	++m_iCount;
}

shared_ptr<CUi_FinalResultSystem> CUi_FinalResultSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_FinalResultSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_FinalResultSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_FinalResultSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_FinalResultSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_FinalResultSystem
	{
		MakeSharedEnabler(const CUi_FinalResultSystem& rhs) : CUi_FinalResultSystem(rhs) { }
	};

	shared_ptr<CUi_FinalResultSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
