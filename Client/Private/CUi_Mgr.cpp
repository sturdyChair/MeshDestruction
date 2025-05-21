#pragma once
#include "CUi_Mgr.h"
#include "CUi.h"
#include "GameInstance.h"


IMPLEMENT_SINGLETON(CUi_Mgr)

CUi_Mgr::CUi_Mgr()
{

}

CUi_Mgr::~CUi_Mgr()
{
	Free();
}

void CUi_Mgr::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

void CUi_Mgr::PriorityTick(_float fTimeDelta)
{

}

void CUi_Mgr::Tick(_float fTimeDelta)
{
    auto iter = m_UiMap.begin();

    for (iter; iter != m_UiMap.end();)
    {
        if (iter->second->Is_Dead())
        {
            m_UiMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

void CUi_Mgr::LateTick(_float fTimeDelta)
{

}

void CUi_Mgr::Tick_Engine(_float fTimeDelta)
{
	PriorityTick(fTimeDelta);
	Tick(fTimeDelta);
	LateTick(fTimeDelta);
}

void CUi_Mgr::Render()
{

}

void CUi_Mgr::Add_Clone(const wstring& UiTag, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
    auto iter = m_UiMap.find(UiTag);

    if (iter != m_UiMap.end())
    {
        if (iter->second->Is_Dead() == false)
        {
            iter->second->Dead();
        }
        m_UiMap.erase(iter);
    }

    m_UiMap.emplace(UiTag, static_pointer_cast<CUi>
       (GAMEINSTANCE->Add_Clone_Return(iLevelIndex, strLayerTag, strPrototypeTag, pArg)));
}

shared_ptr<class CUi> CUi_Mgr::Add_Clone_Return(const wstring& UiTag, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
    auto iter = m_UiMap.find(UiTag);

    if (iter != m_UiMap.end())
    {
        if (iter->second->Is_Dead() == false)
        {
            iter->second->Dead();
        }
        m_UiMap.erase(iter);
    }
    shared_ptr<class CUi> pInstance = static_pointer_cast<CUi>
        (GAMEINSTANCE->Add_Clone_Return(iLevelIndex, strLayerTag, strPrototypeTag, pArg));
    m_UiMap.emplace(UiTag, pInstance);
    return pInstance;
}

void CUi_Mgr::Ui_Dead(const wstring& UiTag)
{
   auto iter = m_UiMap.find(UiTag);

   if (iter == m_UiMap.end())
       return;

   iter->second->Dead();
   m_UiMap.erase(iter);
}

void CUi_Mgr::Ui_Respawn(_uint iLevelIndex, const wstring& strLayerTag, const wstring& UiTag)
{
    auto iter = m_UiMap.find(UiTag);

    if (iter == m_UiMap.end())
        assert(false);
    
    (iter->second)->Set_Dead(false);
    (iter->second)->Respawn();
    GAMEINSTANCE->Push_Object(iLevelIndex, strLayerTag, iter->second);
}

void CUi_Mgr::Set_Active(const wstring& UiTag, _bool Active)
{
    auto iter = m_UiMap.find(UiTag);

    if (iter == m_UiMap.end())
        assert(false);
    
    (iter->second)->Set_Active(Active);
}

void CUi_Mgr::Ui_AllDead()
{
    for (auto& iter : m_UiMap)
    {
        (iter.second)->Dead();
    }
}

shared_ptr<class CUi> CUi_Mgr::Find_UI(const wstring& wstrName)
{
    auto iter = m_UiMap.find(wstrName);

    if (iter == m_UiMap.end())
        assert(false);

    return iter->second;
}

void CUi_Mgr::Free()
{
    m_UiMap.clear();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

//if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_G))
//{
//    Ui_AllDead();
//}
//if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_H))
//{
//    Ui_Respawn(LEVEL_TEST, L"CUi", L"CUi_Orb");
//}