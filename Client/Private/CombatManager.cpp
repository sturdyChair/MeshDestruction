#pragma once
#include "CombatManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCombat_Manager)

CCombat_Manager::CCombat_Manager()
{

}

CCombat_Manager::~CCombat_Manager()
{
    Free();
}

void CCombat_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

}

void CCombat_Manager::PriorityTick(_float fTimeDelta)
{

}

void CCombat_Manager::Tick(_float fTimeDelta)
{

}

void CCombat_Manager::LateTick(_float fTimeDelta)
{

}

void CCombat_Manager::Tick_Engine(_float fTimeDelta)
{
    PriorityTick(fTimeDelta);
    Tick(fTimeDelta);
    LateTick(fTimeDelta);
}

void CCombat_Manager::Render()
{

}

void CCombat_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
