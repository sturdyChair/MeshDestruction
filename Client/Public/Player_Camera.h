#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Player : public CCamera
{
public:
	struct CAMERA_FREE_DESC : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = {};
	};

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);

public:
	~CCamera_Player();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	void Set_Lock_On_Target(shared_ptr<CGameObject> pTarget) { m_pLock_On_Target = pTarget; }
	shared_ptr<CGameObject> Get_Lock_On_Target() {return m_pLock_On_Target; }
	void Start_Following_Bone(_float4x4* pMatrix);
	void End_Following_Bone();

private:
	void Track_Player(_float fTimeDelta);
	void Follow_Bone(_float fTimeDelta);
	void Interpolate(_float fTimeDelta);
	shared_ptr<CGameObject> Auto_LockOn_Target(vector<shared_ptr<CGameObject>> vecTargetList);
private:
	_float m_fMouseSensor = {};
	_float m_fDistance = {2.6f};
	_float m_fDistanceTarget = {2.6f};
	_float3 m_vPitchYawRoll = {};
	_float3 m_vPitchYawRollTarget = {};

	_float m_fInputTimer = 0.f;

	_float  m_fInterpolateSpeed = 7.f;

	shared_ptr<CGameObject> m_pPlayer = nullptr;
	shared_ptr<CGameObject> m_pLock_On_Target = nullptr;
	_float4x4* m_pBoneToFollow = nullptr;
	_bool m_bFollowBone = false;
	_float m_fEnteringBoneTimer = 0.f;
public:
	static shared_ptr<CCamera_Player> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END