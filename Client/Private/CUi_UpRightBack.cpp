#include "CUi_UpRightBack.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "iostream"

wstring CUi_UpRightBack::ObjID = L"CUi_UpRightBack";

CUi_UpRightBack::CUi_UpRightBack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Default(pDevice, pContext)
{
}

CUi_UpRightBack::CUi_UpRightBack(const CUi_2D& rhs)
	:CUi_Default(rhs)
{
}

CUi_UpRightBack::~CUi_UpRightBack()
{
}

HRESULT CUi_UpRightBack::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBackUpRight",
			L"../Bin/Resources/Textures/Ui/Store/StoreBackUpRight.png"));

	return S_OK;
}

HRESULT CUi_UpRightBack::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	Initialize_Gear();
	Set_Font();

	return S_OK;
}

void CUi_UpRightBack::PriorityTick(_float fTimeDelta)
{
}

void CUi_UpRightBack::Tick(_float fTimeDelta)
{
	Adjust_Alpha(fTimeDelta);

	_uint Gear = PLAYERMANAGER->Get_PlayerGear();
	m_FontDesc.String = to_wstring(Gear);
}

void CUi_UpRightBack::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_FontDesc.DirectPos = { 585 - m_FontDesc.String.size() * 7.5f, 330 };
		m_pGear->Set_Pos(585 - m_FontDesc.String.size() * 7.5f - 20, m_pGear->Get_Pos().y);
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_UpRightBack::Render()
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

	return S_OK;
}

void CUi_UpRightBack::Set_Color(_float4 Color)
{
	m_fColor = Color;
	m_FontDesc.Color = Color;
	m_pGear->Set_Color(Color);
}

void CUi_UpRightBack::Set_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x = Color;
		m_FontDesc.Color.x = Color;
		break;
	case ColorType::G:
		m_fColor.y = Color;
		m_FontDesc.Color.y = Color;
		break;
	case ColorType::B:
		m_fColor.z = Color;
		m_FontDesc.Color.z = Color;
		break;
	case ColorType::A:
		m_fColor.w = Color;
		m_FontDesc.Color.w = Color;
		break;
	default:
		assert(false);
		break;
	}
	m_pGear->Set_Color(Type, Color);
}

void CUi_UpRightBack::Add_Color(_float4 Color)
{
	m_fColor.x += Color.x;
	m_fColor.y += Color.y;
	m_fColor.z += Color.z;
	m_fColor.w += Color.w;
	m_FontDesc.Color.x += Color.x;
	m_FontDesc.Color.y += Color.y;
	m_FontDesc.Color.z += Color.z;
	m_FontDesc.Color.w += Color.w;
	m_pGear->Add_Color(Color);
}

void CUi_UpRightBack::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		m_FontDesc.Color.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		m_FontDesc.Color.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		m_FontDesc.Color.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		m_FontDesc.Color.w += Color;
		break;
	default:
		assert(false);
		break;
	}
	m_pGear->Add_Color(Type, Color);
}

void CUi_UpRightBack::Set_Font()
{
	_uint Gear = PLAYERMANAGER->Get_PlayerGear();
	m_FontDesc.DirectPos = { 1100, 300 };
	m_FontDesc.Size = 0.4f;
	m_FontDesc.String = to_wstring(Gear);
}

void CUi_UpRightBack::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pGear->Set_Active(Active);
}

void CUi_UpRightBack::Initialize_Gear()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 40;
	Desc.fSizeY = 40;
	Desc.fX = 585 - m_FontDesc.String.size() * 15.f - 20;
	Desc.fY = 325;
	Desc.fZ = 0.6f;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_Gear";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pGear = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_UpRightBack::Adjust_Alpha(_float fTimeDelta)
{
	if (m_fAlphaTime < 1)
	{
		m_fAlphaTime += fTimeDelta;
		Add_Color(ColorType::A, fTimeDelta * 2);
	}
}

shared_ptr<CUi_UpRightBack> CUi_UpRightBack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_UpRightBack
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_UpRightBack(pDevice, pContext) { }
	};

	shared_ptr<CUi_UpRightBack> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_UpRightBack::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_UpRightBack
	{
		MakeSharedEnabler(const CUi_UpRightBack& rhs) : CUi_UpRightBack(rhs) { }
	};

	shared_ptr<CUi_UpRightBack> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
