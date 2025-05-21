#include "PipeLine.h"
#include "Camera.h"
CPipeLine::CPipeLine()
{
}

CPipeLine::~CPipeLine()
{
	Free();
}

void CPipeLine::Update()
{
	if (m_pMainCamera)
	{
		m_pMainCamera->Set_Transform();
	}

	_matrix ViewInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[TS_VIEW]));
	XMStoreFloat4x4(&m_TransformationMatrices[TS_VIEW_INV], ViewInv);
	XMStoreFloat4x4(&m_TransformationMatrices[TS_PROJ_INV], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[TS_PROJ])));
	XMStoreFloat4(&m_vCamPosition, ViewInv.r[3]);
	XMStoreFloat4(&m_vCamLook, XMVector3Normalize(ViewInv.r[2]));
}

void CPipeLine::Set_Transform(PIPELINE_STATE eState, _fmatrix Matrix)
{
	XMStoreFloat4x4(&m_TransformationMatrices[eState], Matrix);
}

void CPipeLine::Set_Transform(PIPELINE_STATE eState, const _float4x4& Matrix)
{
	m_TransformationMatrices[eState] = Matrix;
}

_fmatrix CPipeLine::Get_Transform_Matrix(PIPELINE_STATE eState) const
{
	return XMLoadFloat4x4(&m_TransformationMatrices[eState]);
}

const _float4x4& CPipeLine::Get_Transform_Float4x4(PIPELINE_STATE eState) const
{
	return m_TransformationMatrices[eState];
}

const _float4& CPipeLine::Get_CamPosition_Float4() const
{
	return m_vCamPosition;
}

_fvector CPipeLine::Get_CamPosition_Vector() const
{
	return XMLoadFloat4(&m_vCamPosition);
}

const _float4& CPipeLine::Get_CamLook_Float4() const
{
	return m_vCamLook;
}

_fvector CPipeLine::Get_CamLook_Vector() const
{
	return XMLoadFloat4(&m_vCamLook);
}

void CPipeLine::Add_Camera(const _wstring& strTag, shared_ptr<CCamera> pCamera)
{
	if (m_Cameras.count(strTag))
		return;
	m_Cameras[strTag] = pCamera;
}

shared_ptr<CCamera> CPipeLine::Get_Camera(const _wstring& strTag)
{
	if (!m_Cameras.count(strTag))
		return nullptr;
	return m_Cameras[strTag];
}

shared_ptr<CCamera> CPipeLine::Get_MainCamera()
{
	return m_pMainCamera;
}

void CPipeLine::Set_MainCamera(const _wstring& strTag)
{
	m_pMainCamera = Get_Camera(strTag);
	if (m_pMainCamera)
	{
		m_strMainCamTag = strTag;
	}
	else
	{
		m_strMainCamTag = L"";
	}
}

void CPipeLine::Remove_Camera(const _wstring& strTag)
{
	if (!m_Cameras.count(strTag))
		return;
	if (strTag == m_strMainCamTag)
	{
		m_pMainCamera = nullptr;
	}
	m_Cameras.erase(strTag);
}

void CPipeLine::Clear_Camera()
{
	m_pMainCamera = nullptr;
	m_Cameras.clear();
}


shared_ptr<CPipeLine> CPipeLine::Create()
{
	MAKE_SHARED_ENABLER(CPipeLine)

	return make_shared<MakeSharedEnabler>();
}

void CPipeLine::Free()
{
	Clear_Camera();
}
