#pragma once
#include "Effect_Manager.h"
#include "Gameinstance.h"
#include "Effect_Union.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{

}

CEffect_Manager::~CEffect_Manager()
{
	Free();
}

void CEffect_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

}

void CEffect_Manager::PriorityTick(_float fTimeDelta)
{

}

void CEffect_Manager::Tick(_float fTimeDelta)
{
	DebugMode();
}

void CEffect_Manager::LateTick(_float fTimeDelta)
{

}

void CEffect_Manager::Tick_Engine(_float fTimeDelta)
{
	PriorityTick(fTimeDelta);
	Tick(fTimeDelta);
	LateTick(fTimeDelta);
}

void CEffect_Manager::Render()
{

}

void CEffect_Manager::Add_Union_Effect(const string& _strKey, shared_ptr<CEffect_Union> _pEffect, _uint _iPoolNum)
{
	if (m_mapEffects.find(_strKey) == m_mapEffects.end())
	{
		shared_ptr<CEffect_Union> pEffect = _pEffect->Create(m_pDevice, m_pContext);
	}

}

shared_ptr<class CEffect_Union> CEffect_Manager::Call_Union_Effect(const string& _strKey, _float4x4 _worldMat)
{
	return shared_ptr<class CEffect_Union>();
}

void CEffect_Manager::DebugMode()
{

}

void CEffect_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
