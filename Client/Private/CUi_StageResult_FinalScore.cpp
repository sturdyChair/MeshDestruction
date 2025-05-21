#include "CUi_StageResult_FinalScore.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"


wstring CUi_StageResult_FinalScore::ObjID = L"CUi_StageResult_FinalScore";

CUi_StageResult_FinalScore::CUi_StageResult_FinalScore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StageResult_FinalScore::CUi_StageResult_FinalScore(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StageResult_FinalScore::~CUi_StageResult_FinalScore()
{
}

HRESULT CUi_StageResult_FinalScore::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_UnderLine",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_UnderLine.png"));
	
	return S_OK;
}

HRESULT CUi_StageResult_FinalScore::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	Initialize_Font();


	return S_OK;
}

void CUi_StageResult_FinalScore::PriorityTick(_float fTimeDelta)
{
	Appear(fTimeDelta);
}

void CUi_StageResult_FinalScore::Tick(_float fTimeDelta)
{
}

void CUi_StageResult_FinalScore::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_StageResult_FinalScore::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	RenderFont(m_FontDesc);
	RenderFont(m_ScoreFont);

	return S_OK;
}

void CUi_StageResult_FinalScore::Appear(_float fTimeDelta)
{
	m_fAppearTime += fTimeDelta;
	if (m_fAppearTime < 0.3f)
	{
		Add_Color(ColorType::A, fTimeDelta * 5);
		m_FontDesc.Color.x += fTimeDelta * 5;
		m_FontDesc.Color.y += fTimeDelta * 5;
		m_FontDesc.Color.w += fTimeDelta * 5;
	}
	else if (0.3f <= m_fAppearTime && m_fAppearTime < 0.7f)
	{
		m_ScoreFont.Size -= fTimeDelta * 4.f;
		if (m_ScoreFont.Size < 1.4f)
			m_ScoreFont.Size = 1.4f;

		m_ScoreFont.Color.x += fTimeDelta * 4.f;
		m_ScoreFont.Color.y += fTimeDelta * 4.f;
		m_ScoreFont.Color.z += fTimeDelta * 4.f;
		m_ScoreFont.Color.w += fTimeDelta * 4.f;
	}
	else
	{
		m_ScoreFont.Size = 1.4f;
	}
}


void CUi_StageResult_FinalScore::Initialize_Font()
{
	m_FontDesc.String = L"최종 결과";
	m_FontDesc.Color = { 0, 0, 0, 0 };
	m_FontDesc.Rotation = XMConvertToRadians(-3);
	m_FontDesc.DirectPos = { Get_Pos().x - Get_Scale().x * 0.4f, Get_Pos().y + 45 };
	m_FontDesc.Size = 1;


	vector<_uint> Score = PLAYERMANAGER->Get_CorusScore();
	_uint SumScore = { 0 };
	for (auto& iter : Score)
		SumScore += iter;
	m_ScoreFont.Size = 2.1f;
	m_ScoreFont.DirectPos = { Get_Pos().x - Get_Scale().x * 0.3f, Get_Pos().y - 15 };
	m_ScoreFont.Rotation = XMConvertToRadians(-3);
	Num_Quotation(m_ScoreFont, SumScore);


}

shared_ptr<CUi_StageResult_FinalScore> CUi_StageResult_FinalScore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StageResult_FinalScore
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StageResult_FinalScore(pDevice, pContext) { }
	};

	shared_ptr<CUi_StageResult_FinalScore> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StageResult_FinalScore::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StageResult_FinalScore
	{
		MakeSharedEnabler(const CUi_StageResult_FinalScore& rhs) : CUi_StageResult_FinalScore(rhs) { }
	};

	shared_ptr<CUi_StageResult_FinalScore> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
