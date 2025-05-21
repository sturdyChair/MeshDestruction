#include "CUi_StoreDesc.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_StoreDesc::ObjID = L"CUi_StoreDesc";

CUi_StoreDesc::CUi_StoreDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StoreDesc::CUi_StoreDesc(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StoreDesc::~CUi_StoreDesc()
{
}

HRESULT CUi_StoreDesc::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreDesc",
			L"../Bin/Resources/Textures/Ui/Store/StoreDesc.png"));

	return S_OK;
}

HRESULT CUi_StoreDesc::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	Initialize_Font();

	return S_OK;
}

void CUi_StoreDesc::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreDesc::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Adjust_Alpha(fTimeDelta);
	}
}

void CUi_StoreDesc::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_StoreDesc::Render()
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

	RenderFont(m_TitleFont, 0);
	RenderFont(m_PriceFont, 0);
	RenderFont(m_BuyFont, 0);
	RenderFont(m_ExplainFont, 20, m_iExplainFontEnterCount);

	return S_OK;
}

void CUi_StoreDesc::Set_Desc(AttackDesc AttackInfo)
{
	m_PriceFont.String = AttackInfo.PriceFont;
	m_ExplainFont.String = AttackInfo.ExplainFont;
	m_TitleFont.String = AttackInfo.TitleFont;
	m_iExplainFontEnterCount = AttackInfo.iEnterCount;
}

void CUi_StoreDesc::Set_Desc(SkillDesc SkillInfo)
{
	m_PriceFont.String = SkillInfo.PriceFont;
	m_ExplainFont.String = SkillInfo.ExplainFont;
	m_TitleFont.String = SkillInfo.TitleFont;
	m_iExplainFontEnterCount = SkillInfo.iEnterCount;
}

void CUi_StoreDesc::Set_Desc(ItemDesc ItemInfo)
{
	m_PriceFont.String = ItemInfo.PriceFont;
	m_ExplainFont.String = ItemInfo.ExplainFont;
	m_TitleFont.String = ItemInfo.TitleFont;
	m_iExplainFontEnterCount = ItemInfo.iEnterCount;
}

void CUi_StoreDesc::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		m_TitleFont.Color.x += Color;
		m_ExplainFont.Color.x += Color;
		m_PriceFont.Color.x += Color;
		m_BuyFont.Color.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		m_TitleFont.Color.y += Color;
		m_ExplainFont.Color.y += Color;
		m_PriceFont.Color.y += Color;
		m_BuyFont.Color.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		m_TitleFont.Color.z += Color;
		m_ExplainFont.Color.z += Color;
		m_PriceFont.Color.z += Color;
		m_BuyFont.Color.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		m_TitleFont.Color.w += Color;
		m_ExplainFont.Color.w += Color;
		m_PriceFont.Color.w += Color;
		m_BuyFont.Color.w += Color;
		break;
	default:
		assert(false);
		break;
	}

}

void CUi_StoreDesc::Set_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x = Color;
		m_TitleFont.Color.x = Color;
		m_ExplainFont.Color.x = Color;
		m_PriceFont.Color.x = Color;
		m_BuyFont.Color.x = Color;
		break;
	case ColorType::G:
		m_fColor.y = Color;
		m_TitleFont.Color.y = Color;
		m_ExplainFont.Color.y = Color;
		m_PriceFont.Color.y = Color;
		m_BuyFont.Color.y = Color;
		break;
	case ColorType::B:
		m_fColor.z = Color;
		m_TitleFont.Color.z = Color;
		m_ExplainFont.Color.z = Color;
		m_PriceFont.Color.z = Color;
		m_BuyFont.Color.z = Color;
		break;
	case ColorType::A:
		m_fColor.w = Color;
		m_TitleFont.Color.w = Color;
		m_ExplainFont.Color.w = Color;
		m_PriceFont.Color.w = Color;
		m_BuyFont.Color.w = Color;
		break;
	default:
		assert(false);
		break;
	}

}

void CUi_StoreDesc::Adjust_Alpha(_float fTimeDelta)
{
	m_fAlphaTime += fTimeDelta;
	if (m_fAlphaTime < 0.5f)
	{
		Add_Color(ColorType::A, fTimeDelta * 4);
	}
}

void CUi_StoreDesc::Set_Active(_bool Active)
{
	m_bActive = Active;
	if (m_bActive)
	{
		Set_Color(ColorType::A, 0);

		m_fAlphaTime = 0;
	}
}

void CUi_StoreDesc::Initialize_Font()
{
	m_TitleFont.DirectPos = { Get_Pos().x - Get_Scale().x * 0.5f + 40, Get_Pos().y + Get_Scale().y * 0.47f};
	m_TitleFont.Size = 0.7f;
	m_TitleFont.String = L"제목";

	m_PriceFont.DirectPos = { Get_Pos().x + 40, Get_Pos().y + Get_Scale().y * 0.35f };
	m_PriceFont.Size = 0.5f;
	m_PriceFont.String = L"가격";

	m_BuyFont.DirectPos = { Get_Pos().x + 60, Get_Pos().y + Get_Scale().y * 0.3f - 20 };
	m_BuyFont.Size = 0.5f;
	m_BuyFont.String = L"구입(길게 누르기)";

	m_ExplainFont.DirectPos = { Get_Pos().x - Get_Scale().x * 0.5f + 35, Get_Pos().y - 20 };
	m_ExplainFont.Size = 0.35f;
	m_ExplainFont.String = L"무라사키";
}

shared_ptr<CUi_StoreDesc> CUi_StoreDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreDesc
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreDesc(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreDesc> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreDesc::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreDesc
	{
		MakeSharedEnabler(const CUi_StoreDesc& rhs) : CUi_StoreDesc(rhs) { }
	};

	shared_ptr<CUi_StoreDesc> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
