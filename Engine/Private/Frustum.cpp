#include "..\Public\Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{

}

CFrustum::~CFrustum()
{
	Free();
}

HRESULT CFrustum::Initialize()
{
	/* 투영스페이스 상의 여덟개 점의 정보를 채워주자. */
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	/* 투영공간상에 구해놓은 여덟개 점을 월드까지 다 치환해놓자. */
	_vector			vPoints[8];

	for (size_t i = 0; i < 8; i++)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::TS_PROJ_INV));
		vPoints[i] = XMVector3TransformCoord(vPoints[i], m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::TS_VIEW_INV));

		XMStoreFloat3(&m_vWorldPoints[i], vPoints[i]);
	}

	/* 월드까지 변환해놓은 점 여덟개로 평면 여섯개를 만든다. */
	Make_Planes(m_vWorldPoints, m_vWorldPlanes);
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	_matrix		WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);

	_float3		vLocalPoints[8];

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&vLocalPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), WorldMatrixInverse));
	}

	Make_Planes(vLocalPoints, m_vLocalPlanes);
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos)))
			return false;
	}
	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vLocalPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vLocalPos)))
			return false;
	}
	return true;
}

void CFrustum::Make_Planes(const _float3* pPoints, _float4* pPlanes)
{
	/* +x */
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	/* -x */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));

	/* +y */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	/* -y */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));

	/* +z */
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	/* -z */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

shared_ptr<CFrustum> CFrustum::Create()
{
	MAKE_SHARED_ENABLER(CFrustum)
	{}
	shared_ptr<CFrustum> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CFrustum"));
		assert(false);
	}

	return pInstance;
}

void CFrustum::Free()
{

}
