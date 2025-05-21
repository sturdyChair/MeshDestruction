#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPartObject;
class CCharacterController;
END


BEGIN(Client)

class CChai : public CGameObject
{
public:
	static wstring ObjID;

	struct AttacK_Tree
	{
		AttacK_Tree* parent; // 부모 노드
		AttacK_Tree* children[3]; // 자식 노드들

		// 노드의 데이터 (예: 3D 모델의 변환 행렬, 이름 등)
		_bool bActivate;
		_uint iCombo_Level;
		string name;

		// 생성자
		AttacK_Tree(const std::string& nodeName) : parent(nullptr), name(nodeName)
		{
			for (size_t i = 0; i < 3; i++)
			{
				children[i] = nullptr;
			}
			iCombo_Level = 0;
			bActivate = false;
		}

		~AttacK_Tree()
		{
			// 자식 노드들을 해제
			for (AttacK_Tree* child : children)
			{
				if(child)
					delete child;
			}
		}
	};
private:
	enum class CHAI_MOVE_STATE {
		 RUN_START, RUN, DASH, JUMP, IDLE,DAMAGED, MOVE_END
	};
	enum class CHAI_ATTACK_STATE {
		COMBO_A_1, COMBO_A_2, COMBO_A_3, COMBO_A_4, COMBO_B_1, COMBO_C_1, COMBO_C_2, ATTACK_END

	};
	enum ATTACK_TREE_TYPE {
		L, R, REST, ATTACK_TYPE_END
	};
private:
	CChai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChai(const CChai& rhs);
public:
	virtual ~CChai();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller);
	virtual void TakeDamage(const DamageInfo& damageInfo);

 private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };

private:
	HRESULT Ready_Components();
	void	Create_Attack_Tree();
	void	Delete_Attack_Tree(AttacK_Tree* node);
	void	Initialize_Attack_Tree(AttacK_Tree* node);

	void	PrepareCallbacks();
public:
	static shared_ptr<CChai> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

private:
	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Turn(_float fTimeDelta);
	_bool Attack(_float fTimeDelta);
	void Attack_Turn(_float fTimeDelta);

	void Adjust_Idle_Timing();
	_bool Find_R();
	void Traverse_AttacK_Tree(AttacK_Tree* node, ATTACK_TREE_TYPE eAttackType, vector<ATTACK_TREE_TYPE> vecCombo);
private:
	_int m_iAnimIndex = 2;
	_bool m_bDebugBool = false;

	vector<string> m_vecAnimList;

	CHAI_MOVE_STATE m_eMoveState = CHAI_MOVE_STATE::MOVE_END;
	vector <shared_ptr< CPartObject>> m_pWeapon ;

	_float m_fLookDiffDegree = 0.f;
	_float m_fTurnTimer = 0.1f;
	_bool m_bTrunRight = false;
	_bool m_bTurning = false;
	_bool m_bAttacKTurning = false;

	_float m_fAttack_Motion_Offset = 0.f;
	_float m_fIdle_Motion_Offset = 0.f;

	_float m_fAttackTimer = 0.0f;
	_bool m_bAttack_Input = false;
	AttacK_Tree* m_pTree_Attack = nullptr;
	vector<ATTACK_TREE_TYPE> m_vecCombo = {};
	string m_strCombo = {};
	ATTACK_TREE_TYPE m_eNextAttack = ATTACK_TREE_TYPE::ATTACK_TYPE_END;
	_bool m_bNextAttack = false;
private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };
	_bool m_isAttackOn = false;
	const _float4x4* m_pMat_RightHand = { nullptr };
	const _float4x4* m_pMat_Spine = { nullptr };
};

END