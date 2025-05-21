#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp
{
private:
	CMainApp();

public:
	~CMainApp();

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	shared_ptr<CGameInstance>			m_pGameInstance = { nullptr };
	shared_ptr<class CPlayer_Manager>	m_pPlayerManager = { nullptr };
	shared_ptr<class CGame_Manager>		m_pGameManager = { nullptr };
	shared_ptr<class CImgui_Manager>	m_pImguiManager = { nullptr };
	shared_ptr<class CUi_Mgr>			m_pCUiMgr = { nullptr };
	shared_ptr<class CBeat_Manager>		m_pBeat_Manager = { nullptr };
	shared_ptr<class CCombat_Manager>	m_pCombat_Manager = { nullptr };
	shared_ptr<class CEffect_Manager>	m_pEffect_Manager = { nullptr };

private:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototoype_Component_ForStatic();

	HRESULT Ready_Prototoype_Objects();

public:
	static shared_ptr<CMainApp> Create();
	void Free();
};

END