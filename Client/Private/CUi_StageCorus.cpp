#include "CUi_StageCorus.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"


wstring CUi_StageCorus::ObjID = L"CUi_StageCorus";

CUi_StageCorus::CUi_StageCorus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StageCorus::CUi_StageCorus(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StageCorus::~CUi_StageCorus()
{
}

HRESULT CUi_StageCorus::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Corus",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Corus.png"));

	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	return S_OK;
}

HRESULT CUi_StageCorus::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);
	
	Initialize_Rank();
	Initialize_Font(pArg);

	return S_OK;
}

void CUi_StageCorus::PriorityTick(_float fTimeDelta)
{
}

void CUi_StageCorus::Tick(_float fTimeDelta)
{
	Appear(fTimeDelta);
}

void CUi_StageCorus::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_StageCorus::Render()
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
	RenderFont(m_SecondFont);

	return S_OK;
}

void CUi_StageCorus::Appear(_float fTimeDelta)
{
	if (m_fAppearTime >= 0.3f)
		return;
	m_fAppearTime += fTimeDelta;
	Adjust_Alpha(fTimeDelta);
	Adjust_Scale(fTimeDelta);
}

void CUi_StageCorus::Adjust_Alpha(_float fTimeDelta)
{
	Add_Color(ColorType::A, fTimeDelta * 4);
	m_pRank->Add_Color(ColorType::A, fTimeDelta * 4);
	m_FontDesc.Color.x += fTimeDelta * 4;
	m_FontDesc.Color.y += fTimeDelta * 4;
	m_FontDesc.Color.z += fTimeDelta * 4;
	m_FontDesc.Color.w += fTimeDelta * 4;
	m_SecondFont.Color.x += fTimeDelta * 4;
	m_SecondFont.Color.y += fTimeDelta * 4;
	m_SecondFont.Color.z += fTimeDelta * 4;
	m_SecondFont.Color.w += fTimeDelta * 4;
}

void CUi_StageCorus::Adjust_Scale(_float fTimeDelta)
{
	Add_Scale(-fTimeDelta * m_fOriginScale.x * 2.5f, -fTimeDelta * m_fOriginScale.y * 2.5f);
	if (Get_Scale().x < m_fOriginScale.x)
	{
		Set_Scale(m_fOriginScale.x, m_fOriginScale.y);
	}
	m_pRank->Add_Scale(-fTimeDelta * m_fOriginRankScale.x * 2.5f, -fTimeDelta * m_fOriginRankScale.y * 2.5f);
	if (m_pRank->Get_Scale().x < m_fOriginRankScale.x)
	{
		m_pRank->Set_Scale(m_fOriginRankScale.x, m_fOriginRankScale.y);
	}
}

void CUi_StageCorus::Initialize_Rank()
{
	CUi_Default::Desc Desc;
	Desc.fX = Get_Pos().x + m_fOriginScale.x * 0.35f;
	Desc.fY = Get_Pos().y - m_fOriginScale.y * 0.35f;
	Desc.fSizeX = m_fOriginRankScale.x * 1.4f;
	Desc.fSizeY = m_fOriginRankScale.y * 1.4f;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_Rank";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pRank = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));


	m_pRank->Set_TextureNum(rand()%5 + 1);
}

void CUi_StageCorus::Initialize_Font(void* pArg)
{
	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	wstring StrTemp = L"ÄÚ·¯½º";
	StrTemp += to_wstring(Arg->CorusStage);
	m_FontDesc.String = StrTemp;
	m_FontDesc.DirectPos = { Get_Pos().x - m_fOriginScale.x * 0.3f, Get_Pos().y + m_fOriginScale.y * 0.3f};
	m_FontDesc.Size = 0.25f;


	Num_Quotation(m_SecondFont, Arg->CorusScore);
	m_SecondFont.Size = 0.5f;
	m_SecondFont.Color = { 0, 0, 0, 0 };
	m_SecondFont.DirectPos = { Get_Pos().x - m_fOriginScale.x * 0.35f, Get_Pos().y };
}

shared_ptr<CUi_StageCorus> CUi_StageCorus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StageCorus
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StageCorus(pDevice, pContext) { }
	};

	shared_ptr<CUi_StageCorus> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StageCorus::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StageCorus
	{
		MakeSharedEnabler(const CUi_StageCorus& rhs) : CUi_StageCorus(rhs) { }
	};

	shared_ptr<CUi_StageCorus> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
