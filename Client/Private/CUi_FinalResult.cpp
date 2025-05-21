#include "CUi_FinalResult.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "PlayerManager.h"
#include "iostream"
#include "Beat_Manager.h"


wstring CUi_FinalResult::ObjID = L"CUi_FinalResult";

CUi_FinalResult::CUi_FinalResult(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_FinalResult::CUi_FinalResult(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_FinalResult::~CUi_FinalResult()
{
}

HRESULT CUi_FinalResult::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResult",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResult%d.png", 4));

	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	return S_OK;
}

HRESULT CUi_FinalResult::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, 
		Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	m_eFontType = Arg->eFontType;
	m_fScaleAcc = Get_Scale().y * 7.f;

	Initialize_Font();
	Initialize_BackGround();
	Initialize_Texture();
	if (m_eFontType == FontType::Corus)
	{
		Create_FinalResult();
	}

	return S_OK;
}

void CUi_FinalResult::PriorityTick(_float fTimeDelta)
{

}

void CUi_FinalResult::Tick(_float fTimeDelta)
{
	m_fAppearTime += fTimeDelta;
	if (m_fAppearTime < 0.3f)
	{
		Appear(fTimeDelta);
	}

	if (m_fAppearTime > 4.f)
	{
		Disappear(fTimeDelta);
	}

	Adjust_FontPos();
	Adjust_BackPos();
}

void CUi_FinalResult::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_FinalResult::Render()
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
	RenderFont(m_SecondFontDesc);
	return S_OK;
}

void CUi_FinalResult::Appear(_float fTimeDelta)
{
	Move(MoveDir::Left, fTimeDelta);
	Add_Color(ColorType::A, 3.5f * fTimeDelta);

	if (!m_bBackAdjust)
	{
		m_pBack->Add_Scale(0, m_fScaleAcc * fTimeDelta);

		if (m_pBack->Get_Scale().y > Get_Scale().y)
		{
			m_pBack->Set_Scale(Get_Scale().x, Get_Scale().y);
			Create_Rank();
			if (m_eFontType == FontType::Corus)
			{
				Create_RankString();
			}
			m_bBackAdjust = true;
		}
	}
	else
	{
		m_pBack->Add_Color(ColorType::R, -fTimeDelta * 7.f);
		m_pBack->Add_Color(ColorType::G, -fTimeDelta * 7.f);
		m_pBack->Add_Color(ColorType::B, -fTimeDelta * 7.f);
	}

}

void CUi_FinalResult::Disappear(_float fTimeDelta)
{
	m_fDisappearTime += fTimeDelta;
	if (m_fDisappearTime < 0.1f)
	{
		Set_Speed(200);
		m_pRank->Set_Speed(200);
		if (m_eFontType == FontType::Corus)
		{
			m_pFinalResult->Set_Speed(200);
			m_pRankString->Set_Speed(200);
		}

		Move(MoveDir::Left, fTimeDelta);
		m_pRank->Move(MoveDir::Left, fTimeDelta);

		if (m_eFontType == FontType::Corus)
		{
			m_pFinalResult->Move(MoveDir::Left, fTimeDelta);
			m_pRankString->Move(MoveDir::Left, fTimeDelta);
		}

		Set_Color(ColorType::A, 1.f);
	}
	else
	{
		m_fSpeedAcc += fTimeDelta;
		m_fAcc += 1.f * fTimeDelta;
		Move(MoveDir::Right, m_fSpeedAcc);
		m_pRank->Move(MoveDir::Right, m_fSpeedAcc);
		if (m_eFontType == FontType::Corus)
		{
			m_pFinalResult->Move(MoveDir::Right, m_fSpeedAcc);
			m_pRankString->Move(MoveDir::Right, m_fSpeedAcc);
		}

		Add_Color(ColorType::A, -m_fAcc);
		m_pBack->Set_Color(ColorType::A, Get_Color().w);
		m_FontDesc.Color.x -= m_fAcc;
		m_FontDesc.Color.y -= m_fAcc;
		m_FontDesc.Color.z -= m_fAcc;
		m_FontDesc.Color.w -= m_fAcc;
		m_SecondFontDesc.Color.x -= m_fAcc;
		m_SecondFontDesc.Color.y -= m_fAcc;
		m_SecondFontDesc.Color.z -= m_fAcc;
		m_SecondFontDesc.Color.w -= m_fAcc;
	}
}

void CUi_FinalResult::Initialize_Font()
{
	m_FontDesc.Color = { 1, 1, 1, 1 };
	m_FontDesc.DirectPos = { Get_Pos().x, Get_Pos().y };
	m_FontDesc.Size = 0.4f;
	m_SecondFontDesc.Color = { 1, 1, 1, 1 };
	m_SecondFontDesc.DirectPos = { Get_Pos().x, Get_Pos().y };
	m_SecondFontDesc.Size = 0.4f;
	string Temp = "";
	switch (m_eFontType)
	{
	case FontType::Corus:
		m_FontDesc.String = L"코러스 ";
		m_SecondFontDesc.String = to_wstring(PLAYERMANAGER->Get_PlayerScore());
		break;
	case FontType::Score:
		m_FontDesc.String = L"점수 ";
		m_SecondFontDesc.String = to_wstring(PLAYERMANAGER->Get_PlayerScore());
		break;
	case FontType::JustTiming:
		m_FontDesc.String = L"클리어 시간";
		m_SecondFontDesc.String = to_wstring(CBeat_Manager::Get_Instance()->Get_Timer());
		Temp = to_string(PLAYERMANAGER->Get_JustTimingCount());
		Temp += "s";
		m_SecondFontDesc.String.assign(Temp.begin(), Temp.end());
		wprintf(m_SecondFontDesc.String.c_str());
		break;
	case FontType::ClearTime:
		m_FontDesc.String = L"저스트 타이밍";
		Temp = to_string(PLAYERMANAGER->Get_JustTimingCount());
		Temp += "%";   
		m_SecondFontDesc.String.assign(Temp.begin(), Temp.end());
		wprintf(m_SecondFontDesc.String.c_str());
		break;
	case FontType::Bonus:
		m_FontDesc.String = L"리버브 보너스";
		Temp = "+";
		Temp += to_string(_uint(PLAYERMANAGER->Get_Reverve()));
		m_SecondFontDesc.String.assign(Temp.begin(), Temp.end());
		wprintf(m_SecondFontDesc.String.c_str());
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_FinalResult::Initialize_Texture()
{
	switch (m_eFontType)
	{
	case FontType::Corus:
		m_iTextureNum = 0;
		break;
	case FontType::Score:
		m_iTextureNum = 1;
		break;
	case FontType::JustTiming:
		m_iTextureNum = 1;
		break;
	case FontType::ClearTime:
		m_iTextureNum = 1;
		break;
	case FontType::Bonus:
		m_iTextureNum = 2;
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_FinalResult::Initialize_BackGround()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = Get_Scale().x;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_FinalResultBack";
	Desc.TextureLevel = LEVEL_STATIC;

	m_pBack = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_FinalResult::Adjust_FontPos()
{
	_float3 Pos = Get_Pos();
	_float3 Scale = Get_Scale();
	if (m_eFontType != FontType::Corus)
	{
		m_FontDesc.DirectPos = { Pos.x + Scale.x * 0.5f - 25 , Pos.y + Scale.y * 0.5f - 10 };
		m_SecondFontDesc.DirectPos = { Pos.x + Scale.x * 0.5f - 20, m_FontDesc.DirectPos.y - Scale.y + 10 };
	}
	else
	{
		m_FontDesc.DirectPos = { Pos.x + Scale.x * 0.5f - 130 , Pos.y + Scale.y * 0.5f - 13 };
		m_SecondFontDesc.DirectPos = { m_FontDesc.DirectPos.x, m_FontDesc.DirectPos.y - Scale.y + 10 };
	}
	size_t Count = m_FontDesc.String.size();
	m_FontDesc.DirectPos.x -= Count * 15;
	Count = m_SecondFontDesc.String.size();
	m_SecondFontDesc.DirectPos.x -= Count * 15;
}

void CUi_FinalResult::Adjust_BackPos()
{
	_float3 Pos = Get_Pos();
	_float ScaleY = Get_Scale().y;
	m_pBack->Set_Pos(Pos.x + 10, Pos.y - (ScaleY - 10), 0.9f);
}

void CUi_FinalResult::Create_Rank()
{
	CUi_Default::Desc Desc;
	if (m_eFontType == FontType::Corus)
	{
		Desc.fSizeX = m_RankSize.x * 1.3f;
		Desc.fSizeY = m_RankSize.y * 1.3f;
	}
	else
	{
		Desc.fSizeX = m_RankSize.x;
		Desc.fSizeY = m_RankSize.y;
	}
	
	Desc.fX = 590;
	Desc.fY = Get_Pos().y - Get_Scale().y * 0.5f;
	Desc.fZ = 0.3f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_Rank";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pRank = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	switch (m_eFontType)
	{
	case FontType::Corus:
		m_pRank->Set_TextureNum(5);
		break;
	case FontType::Score:
		m_pRank->Set_TextureNum(4);
		break;
	case FontType::JustTiming:
		m_pRank->Set_TextureNum(3);
		break;
	case FontType::ClearTime:
		m_pRank->Set_TextureNum(2);
		break;
	case FontType::Bonus:
		m_pRank->Set_TextureNum(1);
		break;
	default:
		assert(false);
		break;
	}

}

void CUi_FinalResult::Create_RankString()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 200;
	Desc.fSizeY = 80;
	Desc.fX = m_pRank->Get_Pos().x;
	Desc.fY = m_pRank->Get_Pos().y + 20;
	Desc.fZ = 0.2f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_FinalResultString";
	Desc.TextureLevel = LEVEL_STATIC;

	m_pRankString = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_FinalResult::Create_FinalResult()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 200;
	Desc.fSizeY = 80;
	Desc.fX = 420;
	Desc.fY = Get_Pos().y + Get_Scale().y * 0.5f + 20;
	Desc.fZ = 0.2f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_FinalResultWord";
	Desc.TextureLevel = LEVEL_STATIC;

	m_pFinalResult = static_pointer_cast<CUi_Default>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}


shared_ptr<CUi_FinalResult> CUi_FinalResult::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_FinalResult
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_FinalResult(pDevice, pContext) { }
	};

	shared_ptr<CUi_FinalResult> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_FinalResult::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_FinalResult
	{
		MakeSharedEnabler(const CUi_FinalResult& rhs) : CUi_FinalResult(rhs) { }
	};

	shared_ptr<CUi_FinalResult> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
