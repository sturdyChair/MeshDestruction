#include "SentientGopro.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "CharacterController.h"

wstring CSentientGopro::ObjID = TEXT("CSentientGopro");

CSentientGopro::CSentientGopro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CSentientGopro::CSentientGopro(const CSentientGopro& rhs)
	: CCamera(rhs)
{
}

CSentientGopro::~CSentientGopro()
{
	Free();
}

HRESULT CSentientGopro::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSentientGopro::Initialize(void* pArg)
{
	GOPRO_DESC* pDesc = static_cast<GOPRO_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	Ready_Components();

	return S_OK;
}

void CSentientGopro::PriorityTick(_float fTimeDelta)
{
	_float fFaster = 1.f + m_fMomentum;
	_float fSpeed = m_pTransformCom->Get_Speed();

	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_LCONTROL))
	{
		if (m_fMomentum < 1.f)
		{
			m_fMomentum = 1.f;
		}
		else
		{
			m_fMomentum -= fTimeDelta * 20.f;
		}
		m_vOffset = { 0.f,1.f,0.f };
		m_pCCT->Resize(1.f, 1.f);
	}
	else
	{
		m_fMomentum -= fTimeDelta * 20.f;
		if(m_fMomentum < 0.f)
			m_fMomentum = 0.f;
		m_vOffset = { 0.f,2.f,0.f };
		m_pCCT->Resize(1.f, 2.f);
	}


	if (m_pGameInstance.lock()->Get_KeyDown(DIK_LSHIFT))
	{
		m_fMomentum = 4.f;
	}

	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_A))
	{
		m_pCCT->Move_Land_Left(fSpeed * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_D))
	{
		m_pCCT->Move_Land_Right(fSpeed * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_W))
	{
		m_pCCT->Move_Forward(fSpeed * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_S))
	{
		m_pCCT->Move_Backward(fSpeed * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	{
		m_pCCT->Set_Velocity({0.f, 20.f, 0.f, 0.f});
	}

	_long	MouseMove = {};


	if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_X))
	{
		_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
	}
	if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
	}

	if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	{
		POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
		ClientToScreen(g_hWnd, &ptWindow);
		SetCursorPos(ptWindow.x, ptWindow.y);
	}
	m_pCCT->Update(fTimeDelta);
	m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMLoadFloat3(&m_vOffset));

	__super::PriorityTick(fTimeDelta);
}

void CSentientGopro::Tick(_float fTimeDelta)
{
	/*if (m_pGameInstance.lock()->Get_MouseDown(DIMK_LBUTTON))
	{
		_uint res = m_pGameInstance->Pixel_Picking_Tool(TEXT("Target_PixelPicking"), g_iWinSizeX, g_iWinSizeY, g_hWnd);
		if (res == 0)
			return;

		CImGui_Manager::Get_Instance()->Select_Tool_Object(g_ObjectMap[res]);

	}*/
}

void CSentientGopro::LateTick(_float fTimeDelta)
{
}

HRESULT CSentientGopro::Render()
{
	return S_OK;
}

HRESULT CSentientGopro::Ready_Components()
{
	CCharacterController::CCT_DESC cctDesc{};
	_vector pos = m_pTransformCom->Get_Position();
	cctDesc.pTransform = m_pTransformCom;
	cctDesc.height = 2.0f;
	cctDesc.radius = 1.f;
	cctDesc.position = PxExtendedVec3{ pos.m128_f32[0],pos.m128_f32[1],pos.m128_f32[2] };
	cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
	cctDesc.userData = this;

	m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));
	m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pCCT->Enable_Pseudo_Physics();
	m_pCCT->Enable_Gravity();


	return S_OK;
}

shared_ptr<CSentientGopro> CSentientGopro::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CSentientGopro
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSentientGopro(pDevice, pContext) {}
	};

	shared_ptr<CSentientGopro> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSentientGopro"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CSentientGopro::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CSentientGopro
	{
		MakeSharedEnabler(const CSentientGopro& rhs) : CSentientGopro(rhs) {}
	};

	shared_ptr<CSentientGopro> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSentientGopro"));
		assert(false);
	}

	return pInstance;
}

void CSentientGopro::Free()
{

}
