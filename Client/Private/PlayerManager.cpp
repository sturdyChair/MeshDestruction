#pragma once
#include "PlayerManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPlayer_Manager)

CPlayer_Manager::CPlayer_Manager()
{

}

CPlayer_Manager::~CPlayer_Manager()
{
    Free();
}

void CPlayer_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    for (_uint i = 0; i < 9; ++i)
    {
        m_CorusScore.emplace_back(i * 1234);
    }
   
}

void CPlayer_Manager::PriorityTick(_float fTimeDelta)
{

}

void CPlayer_Manager::Tick(_float fTimeDelta)
{
 
}

void CPlayer_Manager::LateTick(_float fTimeDelta)
{
    
}

void CPlayer_Manager::Tick_Engine(_float fTimeDelta)
{
    PriorityTick(fTimeDelta);
    Tick(fTimeDelta);
    LateTick(fTimeDelta);
}

void CPlayer_Manager::Render()
{

}

void CPlayer_Manager::Sub_PlayerGear(_uint iGear)
{
    if (m_Desc.iPlayerGear < iGear)
        return;

    m_Desc.iPlayerGear -= (_int)iGear;
}

_bool CPlayer_Manager::Is_HasSkill(wstring SkillName)
{
    auto iter = m_PlayerSkill.find(SkillName);
    if (iter == m_PlayerSkill.end())
        return false;

    return true;
}

void CPlayer_Manager::Add_Skill(wstring SkillName)
{
    SkillDesc asdf;
    m_PlayerSkill.emplace(SkillName, asdf);
}

void CPlayer_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
