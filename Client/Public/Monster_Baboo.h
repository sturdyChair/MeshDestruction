#pragma once

#include "MonsterBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CCharacterController;
END


BEGIN(Client)

class CMonster_Baboo : public CMonsterBase
{
public:
	static wstring ObjID;

	friend class CBabooState_Idle;
	friend class CBabooState_Move;
	friend class CBabooState_Attack;
	friend class CBabooState_Damaged;
	friend class CBabooState_Dead;

private:
	enum class EHand {
		LEFT, RIGHT, BOTH, HAND_END
	};

private:
	CMonster_Baboo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Baboo(const CMonster_Baboo& rhs);
public:
	virtual ~CMonster_Baboo();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CCharacterController>	m_pCCT = { nullptr };

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };

private:
	HRESULT Ready_Components();
	void	PrepareCallbacks();

private:
	void MoveStraight(_float fSpeed);
	void MoveRight(_float fSpeed);

public:
	virtual _bool IsNeedToMove() override;

public:
	_bool IsAnimFinished();
	void SetAnim(const string& strAnimName, _bool isLoop = false);
	void SetAnimHard(const string& strAnimName, _bool isLoop = false);


private:
	class CBabooState* m_pCurState = { nullptr };

	EHand m_eHandAttack = EHand::HAND_END;
	_bool m_isAttackOn = false;

	_bool m_isAirState = false;

private:
	const _float m_fShakeMagnitude = 0.08f;
	const _float m_fShakeTime_C = 0.4f;
	_float m_fShakeTime = 0.f;

private:
	const _float4x4* m_pMat_Spine = { nullptr };
	const _float4x4* m_pMat_RHand = { nullptr };
	const _float4x4* m_pMat_LHand = { nullptr };

private:
	_float m_fDistance = 0.f;
	_float4 m_vfDirToPlayer;
	_float m_fAngleDegree = 0.f;

private:
	const _float m_fArmourGauge_C = 10.f;
	_float m_fArmourGauge = m_fArmourGauge_C;

private:
	const _float m_fMaxHp = 10.f;
	_float m_fCurHp = m_fMaxHp;

private:
	const _float m_fTimer_Attack_C = 5.f;
	_float m_fTimer_Attack = m_fTimer_Attack_C;

private:
	const _float m_fDistance_Far_C = 6.f;
	const _float m_fDistance_Near_C = 2.f;


private:
	shared_ptr<class CBarrier> m_pBarrier;

public:
	static shared_ptr<CMonster_Baboo> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END