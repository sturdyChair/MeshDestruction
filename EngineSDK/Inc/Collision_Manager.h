#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CCollision_Manager 
{
public:
	enum COLLIDER_TYPE { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_PROJECTILE, COLLISION_END };

private:
	CCollision_Manager();
public:
	virtual ~CCollision_Manager();


public:
	void Initialize();
	void Tick(_float fTimeDelta);
	void Clear();
	void Render();

	void CollisionClear();

	void CollisionDeleteCheck();

	void Collision_Check();

	void Add_Collider(COLLIDER_TYPE _colliderType, shared_ptr<class CCollider> pCollider);

	void CollisionAccessBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);
	void CollisionAccess(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);

	void CollisionDenyBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);
	void CollisionDeny(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);

	void CollisionFlagClear();
public:
	static shared_ptr<CCollision_Manager> Create();
	void Free();

private:
	bool CollisionAccessFlag[_uint(COLLISION_END)][_uint(COLLISION_END)] = {};

	map<_ull, bool> m_CollisionInfo;
	list<shared_ptr<class CCollider>> m_ListCollider[_uint(COLLISION_END)];

};

END