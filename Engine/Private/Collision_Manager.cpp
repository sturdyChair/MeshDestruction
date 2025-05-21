#pragma once
#include "Collision_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
{

}

CCollision_Manager::~CCollision_Manager()
{
	Free();
}

void CCollision_Manager::Initialize()
{
	CollisionFlagClear();

	CollisionAccessBoth(COLLISION_PLAYER, COLLISION_MONSTER);
	CollisionAccessBoth(COLLISION_PLAYER, COLLISION_PROJECTILE);
	CollisionAccessBoth(COLLISION_MONSTER, COLLISION_PROJECTILE);
}

void CCollision_Manager::Tick(_float fTimeDelta)
{
	Collision_Check();
}

void CCollision_Manager::Clear()
{

}

void CCollision_Manager::Render()
{

}

void CCollision_Manager::CollisionClear()
{
	for (_uint i = 0; i < _uint(COLLISION_END); ++i)
	{
		m_ListCollider[i].clear();
	}
}

void CCollision_Manager::CollisionDeleteCheck()
{
	for (_uint i = 0; i < _uint(COLLISION_END); ++i)
	{
		for (auto iter = m_ListCollider[i].begin(); iter != m_ListCollider[i].end();)
		{
			if ((*iter)->Get_Owner()->Is_Dead() == true || (*iter)->Get_Owner() == nullptr)
			{
				iter = m_ListCollider[i].erase(iter);
			}
			else
				++iter;
		}
	}
}

void CCollision_Manager::Collision_Check()
{
	_float RayDistance = 0.f;
	for (_uint i = 0; i < _uint(COLLISION_END); ++i)
	{
		if (m_ListCollider[i].empty())
			continue;

		//for (auto& pDst : m_ListCollider[i])
		//{
		//	if (pDst->RayIntersect(GAMEINSTANCE->Get_CamPosition_Vector(), GAMEINSTANCE->Get_Transform_Matrix(CPipeLine::TS_VIEW_INV).r[2], RayDistance))
		//	{
		//		pDst->Get_Owner()->Collision_RayHit();
		//	}
		//}

		COLLIDER_ID ID{};
		for (_uint j = i + 1; j < _uint(COLLISION_END); ++j)
		{
			if (CollisionAccessFlag[i][j])
			{
				for (auto& pDst : m_ListCollider[i])
				{
					for (auto& pSrc : m_ListCollider[j])
					{
						auto pDstOwner = pDst->Get_Owner();
						auto pSrcOwner = pSrc->Get_Owner();
						if (pDstOwner == nullptr || pSrcOwner == nullptr)
							continue;

						if (pDstOwner == pSrcOwner)
							continue;

						ID.Left_ID = pSrc->Get_ColliderID();
						ID.Right_ID = pDst->Get_ColliderID();

						auto iter = m_CollisionInfo.find(ID.ID);
						if (iter == m_CollisionInfo.end())
						{
							m_CollisionInfo.insert({ ID.ID, false });
							iter = m_CollisionInfo.find(ID.ID);
						}
						
						if (pSrc->Intersect(pDst))
						{
							if (iter->second)
							{
								if (pDstOwner->Is_Dead() || pSrcOwner->Is_Dead())
								{
									iter->second = false;
									pDstOwner->Collision_Exit(pSrc, pDst);
									pSrcOwner->Collision_Exit(pDst, pSrc);
								}
								else
								{
									pDstOwner->Collision(pSrc, pDst);
									pSrcOwner->Collision(pDst, pSrc);
								}
							}
							else
							{
								iter->second = true;
								pDstOwner->Collision_Enter(pSrc, pDst);
								pSrcOwner->Collision_Enter(pDst, pSrc);
							}
						}
						else
						{
							if (iter->second)
							{
								iter->second = false;
								pDstOwner->Collision_Exit(pSrc, pDst);
								pSrcOwner->Collision_Exit(pDst, pSrc);
							}
						}
						//if (pDst->Intersect(pSrc))
						//	pSrc->Get_Owner()->Collision_Enter(pDst->Get_Owner());


					}
				}
			}
		}
	}
	//CollisionDeleteCheck();
	CollisionClear();
}

void CCollision_Manager::Add_Collider(COLLIDER_TYPE _colliderType, shared_ptr<class CCollider> pCollider)
{
	m_ListCollider[_uint(_colliderType)].emplace_back(pCollider);
}

void CCollision_Manager::CollisionAccessBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = true;
	CollisionAccessFlag[eSrc][eDst] = true;
}

void CCollision_Manager::CollisionAccess(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = true;
}

void CCollision_Manager::CollisionDenyBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = false;
	CollisionAccessFlag[eSrc][eDst] = false;
}

void CCollision_Manager::CollisionDeny(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = false;
}

void CCollision_Manager::CollisionFlagClear()
{
	std::fill(&CollisionAccessFlag[0][0], &CollisionAccessFlag[COLLISION_END][COLLISION_END], false);
}

shared_ptr<CCollision_Manager> CCollision_Manager::Create()
{
	MAKE_SHARED_ENABLER(CCollision_Manager)

	shared_ptr<CCollision_Manager> pInstance = make_shared<MakeSharedEnabler>();

	pInstance->Initialize();

	return pInstance;
}

void CCollision_Manager::Free()
{
}
