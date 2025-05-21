#pragma once
#include "Client_Defines.h"


BEGIN(Client)

class CUi_Mgr
{
	DECLARE_SINGLETON(CUi_Mgr)

private:
	CUi_Mgr();

public:
	~CUi_Mgr();


public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);
	void Render();


public:
	void Add_Clone(const wstring& UiTag, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	shared_ptr<class CUi> Add_Clone_Return(const wstring& UiTag, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);



public:
	void Ui_Dead(const wstring& UiTag);
	void Ui_Respawn(_uint iLevelIndex, const wstring& strLayerTag, const wstring& UiTag);
	void Set_Active(const wstring& UiTag, _bool Active);
	void Ui_AllDead();
	shared_ptr<class CUi> Find_UI(const wstring& wstrName);
	
private:
	map<const wstring, shared_ptr<class CUi>> m_UiMap;


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	void Free();
};

END