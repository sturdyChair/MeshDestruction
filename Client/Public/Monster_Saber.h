#pragma once

#include "MonsterBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;

class CPartObject;
class CCharacterController;
END


BEGIN(Client)

class CMonster_Saber : public CMonsterBase
{
public:
	static wstring ObjID;

	friend class CSaberState_Idle;
	friend class CSaberState_Move;
	friend class CSaberState_Attack;
	friend class CSaberState_Damaged;

private:
	CMonster_Saber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Saber(const CMonster_Saber& rhs);
public:
	virtual ~CMonster_Saber();

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
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel>	m_pModelCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };

private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();

	void	PrepareCallbacks();

public:
	_bool IsAnimFinished();
	void SetAnim(const string& strAnimName, _bool isLoop = false);
	void SetAnimHard(const string& strAnimName, _bool isLoop = false);

private:
	shared_ptr<CPartObject> m_pWeapon;

	class CSaberState* m_pCurState = { nullptr };
	_bool m_isAirState = false;

private:
	_bool		m_isAttackOn = false;
	_float4		m_vCurDir_Attack = { 0.f, 0.f, 0.f, 0.f };
	_float4		m_vPrePos_Attack = { 0.f, 0.f, 0.f, 1.f };

private:
	const _float4x4* m_pMat_RightHand = { nullptr };
	const _float4x4* m_pMat_Spine = { nullptr };

private:
	_float m_fDistance = 0.f;
	_float4 m_vfDirToPlayer;
	_float m_fAngleDegree = 0.f;

private:
	const _float m_fMaxHp = 10.f;
	_float m_fCurHp = m_fMaxHp;


public:
	static shared_ptr<CMonster_Saber> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END