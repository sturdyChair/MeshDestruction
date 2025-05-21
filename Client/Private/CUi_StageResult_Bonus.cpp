#include "CUi_StageResult_Bonus.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "Beat_Manager.h"


wstring CUi_StageResult_Bonus::ObjID = L"CUi_StageResult_Bonus";

CUi_StageResult_Bonus::CUi_StageResult_Bonus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StageResult_Bonus::CUi_StageResult_Bonus(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StageResult_Bonus::~CUi_StageResult_Bonus()
{
}

HRESULT CUi_StageResult_Bonus::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Bonus",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Bonus.png"));

	return S_OK;
}

HRESULT CUi_StageResult_Bonus::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	
	Initialize_Font();

	return S_OK;
}

void CUi_StageResult_Bonus::PriorityTick(_float fTimeDelta)
{
}

void CUi_StageResult_Bonus::Tick(_float fTimeDelta)
{
	Appear(fTimeDelta);
}

void CUi_StageResult_Bonus::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_StageResult_Bonus::Render()
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
	RenderFont(m_ThirdFont);
	RenderFont(m_FourthFont);

	RenderFont(m_StringFirstFont);
	RenderFont(m_StringSecondFont);
	RenderFont(m_StringThirdFont);
	RenderFont(m_StringFourthFont);

	return S_OK;
}

void CUi_StageResult_Bonus::Initialize_Font()
{
	_float PosY = Get_Pos().y - 70;
	_float PosX = Get_Pos().x - Get_Scale().x * 0.3f;

	m_FontDesc.Color = { 0, 0, 0, 0 };
	m_FontDesc.Size = { 0.4f };
	m_FontDesc.String = L"계속하기";
	m_FontDesc.DirectPos = { PosX , PosY + 105 };
	m_FontDesc.Rotation = XMConvertToRadians(-3);

	m_StringFirstFont.Color = { 0, 0, 0, 0 };
	m_StringFirstFont.Size = { 0.4f };
	m_StringFirstFont.String = L"0";
	m_StringFirstFont.DirectPos = { PosX + 230, PosY + 120 };
	m_StringFirstFont.Rotation = XMConvertToRadians(-3);

	m_SecondFont.Color = { 0, 0, 0, 0 };
	m_SecondFont.Size = { 0.4f };
	m_SecondFont.String = L"클리어 시간";
	m_SecondFont.DirectPos = { PosX , PosY + 70 };
	m_SecondFont.Rotation = XMConvertToRadians(-3);

	m_StringSecondFont.Color = { 0, 0, 0, 0 };
	m_StringSecondFont.Size = { 0.4f };
	Cal_Time();
	m_StringSecondFont.DirectPos = { PosX + 230 , PosY + 85 };
	m_StringSecondFont.Rotation = XMConvertToRadians(-3);

	m_ThirdFont.Color = { 0, 0, 0, 0 };
	m_ThirdFont.Size = { 0.4f };
	m_ThirdFont.String = L"저스트 타이밍";
	m_ThirdFont.DirectPos = { PosX , PosY + 35 };
	m_ThirdFont.Rotation = XMConvertToRadians(-3);

	m_StringThirdFont.Color = { 0, 0, 0, 0 };
	m_StringThirdFont.Size = { 0.4f };
	m_StringThirdFont.String = to_wstring(PLAYERMANAGER->Get_JustTimingCount());
	m_StringThirdFont.String += L"%";
	m_StringThirdFont.DirectPos = { PosX + 230 , PosY + 50 };
	m_StringThirdFont.Rotation = XMConvertToRadians(-3);

	m_FourthFont.Color = { 0, 0, 0, 0 };
	m_FourthFont.Size = { 0.4f };
	m_FourthFont.String = L"받은 피해";
	m_FourthFont.DirectPos = { PosX , PosY };
	m_FourthFont.Rotation = XMConvertToRadians(-3);

	m_StringFourthFont.Color = { 0, 0, 0, 0 };
	m_StringFourthFont.Size = { 0.4f };
	m_StringFourthFont.String = to_wstring(PLAYERMANAGER->Get_AllMinusHp());
	m_StringFourthFont.DirectPos = { PosX + 230 , PosY + 15};
	m_StringFourthFont.Rotation = XMConvertToRadians(-3);
}

void CUi_StageResult_Bonus::Appear(_float fTimeDelta)
{
	m_fAppearTime += fTimeDelta;
	if (m_fAppearTime < 0.4f)
	{
		Move(MoveDir::Right, fTimeDelta);
		Adjust_FontPos(fTimeDelta);
		Add_Color(ColorType::A, fTimeDelta * 4.f);
		Adjust_FontColor(fTimeDelta);
	}

}

void CUi_StageResult_Bonus::Adjust_FontColor(_float fTimeDelta)
{
	m_FontDesc.Color.x += fTimeDelta * 4.f;
	m_FontDesc.Color.y += fTimeDelta * 4.f;
	m_FontDesc.Color.z += fTimeDelta * 4.f;
	m_FontDesc.Color.w += fTimeDelta * 4.f;

	m_StringFirstFont.Color.x += fTimeDelta * 4.f;
	m_StringFirstFont.Color.y += fTimeDelta * 4.f;
	m_StringFirstFont.Color.z += fTimeDelta * 4.f;
	m_StringFirstFont.Color.w += fTimeDelta * 4.f;

	m_SecondFont.Color.x += fTimeDelta * 4.f;
	m_SecondFont.Color.y += fTimeDelta * 4.f;
	m_SecondFont.Color.z += fTimeDelta * 4.f;
	m_SecondFont.Color.w += fTimeDelta * 4.f;

	m_StringSecondFont.Color.x += fTimeDelta * 4.f;
	m_StringSecondFont.Color.y += fTimeDelta * 4.f;
	m_StringSecondFont.Color.z += fTimeDelta * 4.f;
	m_StringSecondFont.Color.w += fTimeDelta * 4.f;

	m_ThirdFont.Color.x += fTimeDelta * 4.f;
	m_ThirdFont.Color.y += fTimeDelta * 4.f;
	m_ThirdFont.Color.z += fTimeDelta * 4.f;
	m_ThirdFont.Color.w += fTimeDelta * 4.f;

	m_StringThirdFont.Color.x += fTimeDelta * 4.f;
	m_StringThirdFont.Color.y += fTimeDelta * 4.f;
	m_StringThirdFont.Color.z += fTimeDelta * 4.f;
	m_StringThirdFont.Color.w += fTimeDelta * 4.f;

	m_FourthFont.Color.x += fTimeDelta * 4.f;
	m_FourthFont.Color.y += fTimeDelta * 4.f;
	m_FourthFont.Color.z += fTimeDelta * 4.f;
	m_FourthFont.Color.w += fTimeDelta * 4.f;

	m_StringFourthFont.Color.x += fTimeDelta * 4.f;
	m_StringFourthFont.Color.y += fTimeDelta * 4.f;
	m_StringFourthFont.Color.z += fTimeDelta * 4.f;
	m_StringFourthFont.Color.w += fTimeDelta * 4.f;
}

void CUi_StageResult_Bonus::Adjust_FontPos(_float fTimeDelta)
{
	m_FontDesc.DirectPos.x += fTimeDelta * m_fSpeed;
	m_SecondFont.DirectPos.x += fTimeDelta * m_fSpeed;
	m_ThirdFont.DirectPos.x += fTimeDelta * m_fSpeed;
	m_FourthFont.DirectPos.x += fTimeDelta * m_fSpeed;

	m_StringFirstFont.DirectPos.x += fTimeDelta * m_fSpeed;
	m_StringSecondFont.DirectPos.x += fTimeDelta * m_fSpeed;
	m_StringThirdFont.DirectPos.x += fTimeDelta * m_fSpeed;
	m_StringFourthFont.DirectPos.x += fTimeDelta * m_fSpeed;
}

void CUi_StageResult_Bonus::Cal_Time()
{
	//m_StringSecondFont.String = to_wstring(CBeat_Manager::Get_Instance()->Get_Timer());
	_float Time = CBeat_Manager::Get_Instance()->Get_Timer();
	_uint Minute = _uint(Time / 60.f);
	_uint Second = _uint(Time - Minute * 60.f);
	wstring MinuteTemp = to_wstring(Minute);
	wstring SecondTemp = to_wstring(Second);
	MinuteTemp += L":";
	MinuteTemp += SecondTemp;
	m_StringSecondFont.String = MinuteTemp;
}

shared_ptr<CUi_StageResult_Bonus> CUi_StageResult_Bonus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StageResult_Bonus
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StageResult_Bonus(pDevice, pContext) { }
	};

	shared_ptr<CUi_StageResult_Bonus> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StageResult_Bonus::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StageResult_Bonus
	{
		MakeSharedEnabler(const CUi_StageResult_Bonus& rhs) : CUi_StageResult_Bonus(rhs) { }
	};

	shared_ptr<CUi_StageResult_Bonus> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
