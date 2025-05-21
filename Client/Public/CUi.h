#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)


class CUi abstract : public CGameObject
{
public:
	enum class MoveDir { Right, Left, Down, Up, End };
	enum class ShaderPass { Default, HpBar, Blend, RoundProgress, ShakeVertical,
		BlendPoint, FromRight, FromLeft, Emission, Trail, TrailDecal, 
		TrailDistortion, Sonar, BlockLight, BlockLightEven, End };


public:
	using Desc = struct Ui_DefaultStructInfo
	{
		_float fSizeX = { 1 };
		_float fSizeY = { 1 };
		_float fSizeZ = { 1 };
		_float fX = { 0 };
		_float fY = { 0 };
		_float fZ = { 0.7f };
		_float fSpeed = { 0 };
		_float4 fColor = { 1, 1, 1, 1 };
		wstring ShaderFileName;
		ShaderPass ShaderPass = { ShaderPass::Default };
	};

protected:
	CUi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi(const CUi& rhs);


public:
	~CUi();


public:
	virtual HRESULT Initialize_Prototype()PURE;
	virtual HRESULT Initialize(void* pArg) PURE;
	virtual void PriorityTick(_float fTimeDelta) PURE;
	virtual void Tick(_float fTimeDelta) PURE;
	virtual void LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;


public:
	virtual void Move(MoveDir Dir, _float fTimeDelta) PURE;
	virtual void Rotation(_float fAngle, _fvector vAxis) PURE;
	virtual void Respawn() {};


public:
	void Set_Scale(_float SizeX, _float SizeY, _float SizeZ = 1);
	void Set_Pos(_float fX, _float fY, _float fZ = 0.7f);
	void Set_Speed(_float Speed) { m_fSpeed = Speed; }
	_float Get_Speed() const { return m_fSpeed; }
	_float3 Get_Pos();
	_float3 Get_Scale();
	_float2 Get_CUiDirectCursorPos(HWND HWND);
	_float2 Change_DirectCoordinate(_float fX, _float fY);
	_float2 Change_APICoordinate(_float2 DirectPos);
	virtual void Set_Active(_bool Active) { m_bActive = Active; }


protected:
	virtual HRESULT Ready_Components(void* pArg) PURE;
	void Initialize_Transform();
	void InitializeUi(_float fX, _float fY, _float fZ, _float fSizeX, _float fSizeY, _float fSizeZ = 1, _float fSpeed = 0, _float4 fColor = { 1, 1, 1, 1});


protected:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	_float m_fSpeed = { 0 };
	_float4 m_fColor = { 1, 1, 1, 1 };
	ShaderPass m_eShaderPass = { ShaderPass::Default };
	_bool m_bActive = { true };


public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) PURE;
};

END

//if (m_pGameInstance.lock()->Get_MouseDown(DIMK_LBUTTON))
//{
//	_float2 Pos = Get_CUiDirectCursorPos(g_hWnd);
//	cout << "X: " << Pos.x << endl;
//	cout << "Y: " << Pos.y << endl;
//}