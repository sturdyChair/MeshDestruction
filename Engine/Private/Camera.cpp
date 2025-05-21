#include "Camera.h"
#include "GameInstance.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs) :
	CGameObject(rhs)
{
}

CCamera::~CCamera()
{
	Free();
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	static _uint iCamID = 0;
	if (pArg)
	{
		__super::Initialize(pArg);
		CAMERA_DESC* pDesc = (CAMERA_DESC*)pArg;
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(pDesc->fFovy, pDesc->fAspect, pDesc->fNear, pDesc->fFar));
		m_fAspect = pDesc->fAspect;
		m_fFovy = pDesc->fFovy;
		m_fNear = pDesc->fNear;
		m_fFar = pDesc->fFar;

		m_vEye = pDesc->vEye;
		m_pTransformCom->Set_Position(XMLoadFloat4(&m_vEye));
		m_vAt = pDesc->vAt;
		m_pTransformCom->LookAt(XMLoadFloat4(&m_vAt));
		m_pGameInstance.lock()->Add_Camera(pDesc->strTag, static_pointer_cast<CCamera, CGameObject>(shared_from_this()));
	}
	else
	{
		++iCamID;
		m_pGameInstance.lock()->Add_Camera(to_wstring(iCamID), static_pointer_cast<CCamera, CGameObject>(shared_from_this()));
		CTransform::TRANSFORM_DESC Desc;
		Desc.fRotationPerSec = 1.f;
		Desc.fSpeedPerSec = 1.f;
		__super::Initialize(&Desc);

		m_fAspect = 720.f / 1280.f;
		m_fFovy = XM_PI * 0.33f;
		m_fNear = 0.1f;
		m_fFar = 300.f;
		m_vEye = _float4(0.f, 10.f, -10.f, 1.f);
		m_pTransformCom->Set_Position(XMLoadFloat4(&m_vEye));
		m_vAt = _float4(0.f, 0.f, 0.f, 1.f);
		m_pTransformCom->LookAt(XMLoadFloat4(&m_vAt));
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
	}




	return S_OK;
}

void CCamera::PriorityTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::LateTick(_float fTimeDelta)
{

}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Set_Transform()
{
	m_pGameInstance.lock()->Set_Transform(CPipeLine::PIPELINE_STATE::TS_VIEW, m_pTransformCom->Get_WorldMatrix_Inversed());
	m_pGameInstance.lock()->Set_Transform(CPipeLine::PIPELINE_STATE::TS_PROJ, m_ProjMatrix);
}


void CCamera::Free()
{
}
