#include "CUi.h"

CUi::CUi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CUi::CUi(const CUi& rhs)
	:CGameObject(rhs)
{
}

CUi::~CUi()
{
	Free();
}


void CUi::Set_Scale(_float SizeX, _float SizeY, _float SizeZ)
{
	_float3 Scale = { SizeX, SizeY, SizeZ };
	m_pTransformCom->Set_Scale(XMLoadFloat3(&Scale));
}

void CUi::Set_Pos(_float fX, _float fY, _float fZ)
{
	_float3 Pos = { fX, fY, fZ };
	m_pTransformCom->Set_Position(XMLoadFloat3(&Pos));
}

_float3 CUi::Get_Pos()
{
	_float3 Pos = { 0, 0, 0 };
	XMStoreFloat3(&Pos, m_pTransformCom->Get_Position());
	return Pos;
}

_float3 CUi::Get_Scale()
{
	return m_pTransformCom->Get_Scale();
}

_float2 CUi::Get_CUiDirectCursorPos(HWND HWND)
{
	POINT pPt = {};
	GetCursorPos(&pPt);
	ScreenToClient(HWND, &pPt);
	return Change_DirectCoordinate((float)pPt.x, (float)pPt.y);
}

_float2 CUi::Change_DirectCoordinate(_float fX, _float fY)
{
	_float2 DirectPos = { 0, 0 };
	DirectPos.x = fX - g_iWinSizeX * 0.5f;
	DirectPos.y = -fY + g_iWinSizeY * 0.5f;
	return DirectPos;
}

_float2 CUi::Change_APICoordinate(_float2 DirectPos)
{
	_float2 APIPos = { 0, 0 };
	APIPos.x = DirectPos.x + g_iWinSizeX * 0.5f;
	APIPos.y = -DirectPos.y + g_iWinSizeY * 0.5f;
	return APIPos;
}

void CUi::Initialize_Transform()
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&TransformDesc)))
		assert(false);
}

void CUi::InitializeUi(_float fX, _float fY, _float fZ, _float fSizeX, _float fSizeY, _float fSizeZ, _float fSpeed, _float4 fColor)
{
	Set_Scale(fSizeX, fSizeY, fSizeZ);
	Set_Pos(fX, fY, fZ);
	m_fSpeed = fSpeed;
	m_fColor = fColor;
}
