#include "Tool_Camera.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CCamera_Tool::CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
	
}

CCamera_Tool::CCamera_Tool(const CCamera_Tool& rhs)
	: CCamera(rhs)
{
}

CCamera_Tool::~CCamera_Tool()
{
	Free();
}

HRESULT CCamera_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Tool::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Tool::PriorityTick(_float fTimeDelta)
{
	_float fFaster = m_pGameInstance.lock()->Get_KeyPressing(DIK_LSHIFT) ? 3.f : 1.f;
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta * fFaster);
	}
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_Q))
	{
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + vUp * fTimeDelta * 10.f * fFaster);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_E))
	{
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() - vUp * fTimeDelta * 10.f * fFaster);
	}


	_long	MouseMove = {};

	if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	{
		if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_X))
		{
			_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta  * MouseMove * m_fMouseSensor);
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}
		if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}

	if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	{
		POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
		ClientToScreen(g_hWnd, &ptWindow);
		SetCursorPos(ptWindow.x, ptWindow.y);
	}


	__super::PriorityTick(fTimeDelta);
}

void CCamera_Tool::Tick(_float fTimeDelta)
{
	/*if (m_pGameInstance.lock()->Get_MouseDown(DIMK_LBUTTON))
	{
		_uint res = m_pGameInstance->Pixel_Picking_Tool(TEXT("Target_PixelPicking"), g_iWinSizeX, g_iWinSizeY, g_hWnd);
		if (res == 0)
			return;

		CImGui_Manager::Get_Instance()->Select_Tool_Object(g_ObjectMap[res]);

	}*/
}

void CCamera_Tool::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Tool::Render()
{
	return S_OK;
}

shared_ptr<CCamera_Tool> CCamera_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CCamera_Tool
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCamera_Tool(pDevice, pContext) { }
	};

	shared_ptr<CCamera_Tool> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Tool"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CCamera_Tool::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CCamera_Tool
	{
		MakeSharedEnabler(const CCamera_Tool& rhs) : CCamera_Tool(rhs) { }
	};

	shared_ptr<CCamera_Tool> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Tool"));
		assert(false);
	}

	return pInstance;
}

void CCamera_Tool::Free()
{

}
