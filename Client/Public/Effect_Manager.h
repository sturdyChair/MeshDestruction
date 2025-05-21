#pragma once
#include "Client_Defines.h"

BEGIN(Client)

class CEffect_Manager
{
	DECLARE_SINGLETON(CEffect_Manager)

private:
	CEffect_Manager();

public:
	~CEffect_Manager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);

	void Render();

public:
	void Add_Union_Effect(const string& _strKey, shared_ptr<class CEffect_Union> _pEffect, _uint _iPoolNum);
	shared_ptr<class CEffect_Union> Call_Union_Effect(const string& _strKey, _float4x4 _worldMat);

public:
	void DebugMode();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	
	map<string, shared_ptr<class CEffect_Union>> m_mapEffects;

public:
	void Free();
};

END