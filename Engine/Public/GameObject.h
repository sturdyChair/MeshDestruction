

#pragma once

#include "Transform.h"	
#include "Collision_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public enable_shared_from_this<CGameObject>
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
public:
	virtual ~CGameObject();

public:
	static const _wstring		m_strTransformTag;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	shared_ptr<class CComponent> Find_Component(const wstring& strComponentTag);
	
	shared_ptr<class CTransform> Get_Transform() const { return m_pTransformCom; }
	virtual void Push_From_Pool(void* pArg) {};
	virtual void Set_Dead(bool bDead = true) { m_bIsDead = bDead; }
	void    Dead();
	bool	Is_Dead() const { return m_bIsDead; }
	virtual void Dying() {};

	_uint Get_ObjectID() const { return m_iObjectID; }
	COLLIDER_GROUP Get_CollisionGroup() const { return m_iCollisionGroup; }
	void Set_CollisionGroup(COLLIDER_GROUP iGroup) { m_iCollisionGroup = iGroup; }
	virtual void Collision_RayHit() {}
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) {}
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) {}
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) {}

	virtual void TakeDamage(const DamageInfo& damageInfo) {};
	virtual void Sweeped(const PxSweepHit& hit, const DamageInfo& damageInfo) {};
	virtual void OnPxContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPair, CGameObject* pOther) {};
	template<typename T>
	shared_ptr<class CComponent> Find_Component_By_Type();


protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	shared_ptr<class CTransform> m_pTransformCom = { nullptr };
	weak_ptr<class CGameInstance> m_pGameInstance;
	COLLIDER_GROUP m_iCollisionGroup;
protected:
	bool m_bIsDead = false;
	_uint m_iObjectID = 0;
private:
	static _uint s_iID;
protected:
	map<const wstring, shared_ptr<class CComponent>>		m_Components;

public:
	list<shared_ptr<RESOURCE_INFO>> Get_ResourceInfo() const {
		return m_List_ResourceInfo;
	};
	list<wstring> Get_Owning_Objects() const {
		return m_List_Owning_ObjectID;
	};
	void Set_PoolSize(_uint iPoolSize) {
		m_iPoolSize = iPoolSize;
	};
	_uint Get_PoolSize() const {
		return m_iPoolSize;
	};

protected:
	_uint					m_iLevelIndex{ 0 };

	list<shared_ptr<RESOURCE_INFO>>		m_List_ResourceInfo;
	list<wstring>						m_List_Owning_ObjectID;
private:
	_uint								m_iPoolSize{ 0 };

protected:
	shared_ptr<CComponent> Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, void* pArg = nullptr);

public:
	static unordered_map<_uint, CGameObject*> s_HashedMap;
	static shared_ptr<CGameObject> Find_Object_With_ID(_uint iID);

public:
	shared_ptr<CGameObject> Clone(void* pArg, _uint iLevelIndex);
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	void Free();
};

END


