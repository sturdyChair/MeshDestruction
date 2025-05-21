#pragma once
#include "GameManager.h"
#include "Gameinstance.h"
#include "Imgui_Manager.h"
#include "Texture.h"

IMPLEMENT_SINGLETON(CGame_Manager)

CGame_Manager::CGame_Manager()
{

}

CGame_Manager::~CGame_Manager()
{
	Free();
}

void CGame_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
	m_pDefaultNorm = CTexture::Create(m_pDevice, m_pContext,	TEXT("../Bin/Resources/Textures/DefaultNorm.png",	1));
	m_pDefaultOrm = CTexture::Create(m_pDevice, m_pContext,		TEXT("../Bin/Resources/Textures/DefaultOrm.png",	1));
	m_pInDefaultDiff = CTexture::Create(m_pDevice, m_pContext,	TEXT("../Bin/Resources/Textures/Inner/In_D.png",	1));
	m_pInDefaultNorm = CTexture::Create(m_pDevice, m_pContext,	TEXT("../Bin/Resources/Textures/Inner/In_N.png",	1));
	m_pDefaultNoise = CTexture::Create(m_pDevice, m_pContext,	TEXT("../Bin/Resources/Textures/DefaultNoise.png",	1));
	m_pDefaultRGBNoise = CTexture::Create(m_pDevice, m_pContext,	TEXT("../Bin/Resources/Textures/RGBNoise.png",	1));
	m_pDefaultFlow = CTexture::Create(m_pDevice, m_pContext,	TEXT("../Bin/Resources/Textures/DefaultFlow.png",	1));
	Add_GlobalTexture(L"DirtRockInDiff", CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Cube/In_D.png", 1)) );
	Add_GlobalTexture(L"DirtRockInNorm", CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/Cube/In_N.png", 1)) );
}

void CGame_Manager::PriorityTick(_float fTimeDelta)
{

}

void CGame_Manager::Tick(_float fTimeDelta)
{
	DebugMode();
}

void CGame_Manager::LateTick(_float fTimeDelta)
{

}

void CGame_Manager::Tick_Engine(_float fTimeDelta)
{
	PriorityTick(fTimeDelta);
	Tick(fTimeDelta);
	LateTick(fTimeDelta);
}

void CGame_Manager::Render()
{

}

shared_ptr<class CTexture> CGame_Manager::Find_GlobalTexture(const wstring& strTag)
{
	auto iter = m_GlobalTextures.find(strTag);
	if (iter == m_GlobalTextures.end())
		return nullptr;

	return iter->second;
}

_bool CGame_Manager::Add_GlobalTexture(const wstring& strTag, shared_ptr<class CTexture> pTexture)
{
	if (Find_GlobalTexture(strTag))
		return false;
	m_GlobalTextures.insert({ strTag, pTexture });
	return true;
}

void CGame_Manager::DebugMode()
{
	if (GAMEINSTANCE->Get_KeyDown(DIK_F8)) m_bDebugMode = !m_bDebugMode;

	if(m_bDebugMode == true)
		IMGUIMANAGER->Imgui_SetState(eImgui_State::IMGUI_STATE_RENDER);
	else
		IMGUIMANAGER->Imgui_SetState(eImgui_State::IMGUI_STATE_WAIT);

}

void CGame_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
