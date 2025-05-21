#include "Player_Camera.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "PlayerManager.h"
#include "CombatManager.h"
CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs)
	: CCamera(rhs)
{
}

CCamera_Player::~CCamera_Player()
{
	Free();
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayer = CPlayer_Manager::Get_Instance()->Get_Player();

	return S_OK;
}

void CCamera_Player::PriorityTick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	//{
	//	POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	//	ClientToScreen(g_hWnd, &ptWindow);
	//	SetCursorPos(ptWindow.x, ptWindow.y);
	//}
	Track_Player(fTimeDelta);



	__super::PriorityTick(fTimeDelta);
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	/*if (m_pGameInstance.lock()->Get_MouseDown(DIMK_LBUTTON))
	{
		_uint res = m_pGameInstance->Pixel_Picking_Tool(TEXT("Target_PixelPicking"), g_iWinSizeX, g_iWinSizeY, g_hWnd);
		if (res == 0)
			return;

		CImGui_Manager::Get_Instance()->Select_Tool_Object(g_ObjectMap[res]);

	}*/
}

void CCamera_Player::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

void CCamera_Player::Start_Following_Bone(_float4x4* pMatrix)
{
	m_fEnteringBoneTimer = 0.f;
	m_bFollowBone = true;
	m_pBoneToFollow = pMatrix;
}

void CCamera_Player::End_Following_Bone()
{
	m_fEnteringBoneTimer = 0.1f;
	m_bFollowBone = false;
}

void CCamera_Player::Track_Player(_float fTimeDelta)
{
	m_fInputTimer -= fTimeDelta;

	_long MouseMoveX = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_X);
	if (MouseMoveX != 0)
	{
		m_vPitchYawRollTarget.y += fTimeDelta * MouseMoveX * m_fMouseSensor * 10.f;
		m_fInputTimer = 1.5f;
	}

	//_float fYMovement = m_pGameInstance->Get_DIMouseMove(DIMM_Y);
	m_vPitchYawRollTarget.x -= fTimeDelta * m_pGameInstance.lock()->Get_DIMouseMove(DIMM_Y) * m_fMouseSensor * 10.f;


	if (m_vPitchYawRollTarget.x < -XM_PIDIV2 + 0.01f)
	{
		m_vPitchYawRollTarget.x = -XM_PIDIV2 + 0.01f;
	}
	if (m_vPitchYawRollTarget.x > XM_PIDIV2 - 0.01f)
	{
		m_vPitchYawRollTarget.x = XM_PIDIV2 - 0.01f;
	}
	m_fDistance += fTimeDelta * m_pGameInstance.lock()->Get_DIMouseMove(DIMM_WHEEL) * m_fMouseSensor;
	if (m_fDistanceTarget <= 1.f)
	{
		m_fDistanceTarget = 1.f;
	}
	if (m_fDistanceTarget >= 20.f)
	{
		m_fDistanceTarget = 20.f;
	}
	Interpolate(fTimeDelta);


	auto m_pTargetTransform = m_pPlayer->Get_Transform();
	if (m_pTargetTransform)
	{
		_matrix Matrix = m_pTargetTransform->Get_WorldMatrix_XMMat();
		Matrix.r[3] += XMVectorSet(0, 1.f, 0, 0);
		_vector vOffset = XMVector3Rotate(XMVectorSet(0.f, 0.f, m_fDistance, 0.f), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&m_vPitchYawRoll)));
		_vector vTarget = Matrix.r[3];
		_vector vNormOff = XMVector3Normalize(vOffset);
		_float3 vfPos;
		XMStoreFloat3(&vfPos, vTarget + vNormOff * 2.f);
		PxSweepBuffer Hit;
		PxTransform Start{ PxVec3{vfPos.x,vfPos.y,vfPos.z} };

		_float3 vDirection;

		XMStoreFloat3(&vDirection, vNormOff);
		PxVec3 pvDist = PxVec3{ vDirection.x,vDirection.y,vDirection.z };
		pvDist.normalize();
		if (m_pGameInstance.lock()->PxSweep(Start, pvDist, m_fDistance, 1.f, Hit))
		{
			_float fDistance = Hit.block.distance;
			vOffset = vNormOff * (fDistance + 2.f);
		}


		Matrix.r[3] += vOffset;


		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_EffectOffset) * Matrix);
		m_pTransformCom->Set_WorldMatrix(Matrix);
		m_pTransformCom->LookAt(vTarget);

		m_pLock_On_Target = Auto_LockOn_Target(CCombat_Manager::Get_Instance()->Get_Enemy_Vector());
		if (m_pLock_On_Target && m_fInputTimer <= 0.f)
		{
			auto pLockOnTargetTransform = m_pLock_On_Target->Get_Transform();
			_matrix LockOnMatrix = pLockOnTargetTransform->Get_WorldMatrix_XMMat();

			_vector vLockOnDir = (vTarget - LockOnMatrix.r[3]);
			if (XMVector3Length(vLockOnDir).m128_f32[0] < 0.1f)
				return;
			_float fX = XMVectorGetX(vLockOnDir);
			_float fZ = XMVectorGetZ(vLockOnDir);
			_float fAngle = atan2(fX, fZ);
			//_vector vQuar = m_pGameInstance.lock()->XMQuaternionRotationBetweenVectors(XMVectorSet(0.f, 0.f, m_fDistance, 0.f), -vLockOnDir);
			//_float Pitch, Yaw, Roll;
			//m_pGameInstance.lock()->QuaternionToEulerAngles(vQuar, Yaw , Pitch, Roll);
			//m_vPitchYawRollTarget.x = Pitch;
			//if (fAngle < 0.f)
			//{
			//	fAngle = XM_2PI + fAngle;
			//}
			_float fDiff = fAngle - m_vPitchYawRollTarget.y;
			if (fDiff < 0.f)
			{
				while (fDiff < -XM_PI)
				{
					fAngle += XM_2PI;
					fDiff = fAngle - m_vPitchYawRollTarget.y;
				}
			}
			if (fDiff > 0.f)
			{
				while (fDiff > XM_PI)
				{
					fAngle -= XM_2PI;
					fDiff = fAngle - m_vPitchYawRollTarget.y;
				}
			}
			m_vPitchYawRollTarget.y = fAngle;

		}

	}
	Follow_Bone(fTimeDelta);
}

void CCamera_Player::Follow_Bone(_float fTimeDelta)
{
	if(m_bFollowBone)
	{
		if (m_fEnteringBoneTimer <= 0.1f)
			m_fEnteringBoneTimer += fTimeDelta;
		else
			m_fEnteringBoneTimer = 0.1f;
	}
	else
	{
		if (m_fEnteringBoneTimer >= 0.0f)
			m_fEnteringBoneTimer -= fTimeDelta;
		else
			m_fEnteringBoneTimer = 0.0f;

	}
	if (m_fEnteringBoneTimer <= 0.f || !m_pBoneToFollow)
		return;

	_matrix FollowingBone = XMLoadFloat4x4(m_pBoneToFollow);
	_float fLerp = m_fEnteringBoneTimer / 0.1f;
	_matrix LerpedMatrix = m_pTransformCom->Get_WorldMatrix_XMMat() * (1.f - fLerp) + FollowingBone * fLerp;
	m_pTransformCom->Set_WorldMatrix(LerpedMatrix);

}

void CCamera_Player::Interpolate(_float fTimeDelta)
{

	//while (m_vPitchYawRollTarget.y >= XM_2PI)
	//{
	//	m_vPitchYawRollTarget.y -= XM_2PI;
	//}
	//while (m_vPitchYawRollTarget.y <= 0.f)
	//{
	//	m_vPitchYawRollTarget.y += XM_2PI;
	//}

	if (m_fInterpolateSpeed * fTimeDelta >= 1.f)
	{
		m_vPitchYawRoll.x = m_vPitchYawRollTarget.x;
		m_vPitchYawRoll.y = m_vPitchYawRollTarget.y;
		m_vPitchYawRoll.z = m_vPitchYawRollTarget.z;
		m_fDistance = m_fDistanceTarget;
	}
	else
	{
		_float fYawDiff = m_vPitchYawRollTarget.x - m_vPitchYawRoll.x;
		if (abs(fYawDiff) > 0.01f)
		{
			m_vPitchYawRoll.x += (fYawDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_vPitchYawRoll.x = m_vPitchYawRollTarget.x;
		}
		_float fPitchDiff = m_vPitchYawRollTarget.y - m_vPitchYawRoll.y;
		if (abs(fPitchDiff) > 0.01f)
		{
			m_vPitchYawRoll.y += (fPitchDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_vPitchYawRoll.y = m_vPitchYawRollTarget.y;
		}
		_float fRollDiff = m_vPitchYawRollTarget.z - m_vPitchYawRoll.z;
		if (abs(fPitchDiff) > 0.01f)
		{
			m_vPitchYawRoll.z += (fPitchDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_vPitchYawRoll.z = m_vPitchYawRollTarget.z;
		}

		_float fDistDiff = m_fDistanceTarget - m_fDistance;
		if (abs(fDistDiff) > 0.01f)
		{
			m_fDistance += (fDistDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_fDistance = m_fDistanceTarget;
		}
	}

}

shared_ptr<CGameObject> CCamera_Player::Auto_LockOn_Target(vector<shared_ptr<CGameObject>> vecTargetList)
{
	auto playerTransform = m_pPlayer->Get_Transform();
	_float fDistance = FLT_MAX;
	shared_ptr<CGameObject> lockOnTarget = nullptr;
	for (auto& iter : vecTargetList)
	{
		auto targetTransform = iter->Get_Transform();
	
		_float length = XMVectorGetX(XMVector3Length(targetTransform->Get_Position() - playerTransform->Get_Position()));
	
		if (fDistance > length)
		{
			fDistance = length;
			lockOnTarget = iter;
		}
	}
	return lockOnTarget;
}

shared_ptr<CCamera_Player> CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CCamera_Player
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCamera_Player(pDevice, pContext) { }
	};

	shared_ptr<CCamera_Player> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CCamera_Player::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CCamera_Player
	{
		MakeSharedEnabler(const CCamera_Player& rhs) : CCamera_Player(rhs) { }
	};

	shared_ptr<CCamera_Player> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		assert(false);
	}

	return pInstance;
}

void CCamera_Player::Free()
{

}
