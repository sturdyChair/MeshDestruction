#pragma once
#include "Client_Defines.h"

BEGIN(Client)

class CGame_Manager
{
	DECLARE_SINGLETON(CGame_Manager)

private:
	CGame_Manager();

public:
	~CGame_Manager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);

	void Render();

	void Set_Current_Level(_uint iLevel) { m_iLevel = iLevel; }
	_uint Get_Current_Level()const { return m_iLevel; }

	shared_ptr<class CTexture> Get_DefaultNorm() { return m_pDefaultNorm; }
	shared_ptr<class CTexture> Get_DefaultOrm() { return m_pDefaultOrm; }
	shared_ptr<class CTexture> Get_DefaultNoise() { return m_pDefaultNoise; }
	shared_ptr<class CTexture> Get_DefaultRGBNoise() { return m_pDefaultRGBNoise; }
	shared_ptr<class CTexture> Get_DefaultFlow() { return m_pDefaultFlow; }

	shared_ptr<class CTexture> Get_InDiffuse() { return m_pInDefaultDiff; }
	shared_ptr<class CTexture> Get_InNormal() { return m_pInDefaultNorm; }
	
	shared_ptr<class CTexture> Find_GlobalTexture(const wstring& strTag);
	_bool Add_GlobalTexture(const wstring& strTag, shared_ptr<class CTexture> pTexture);

public:
	void DebugMode();

private:
	map<wstring, shared_ptr<class CTexture>> m_GlobalTextures;
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	shared_ptr<class CTexture> m_pDefaultNorm;
	shared_ptr<class CTexture> m_pDefaultOrm;

	shared_ptr<class CTexture> m_pInDefaultDiff;
	shared_ptr<class CTexture> m_pInDefaultNorm;

	shared_ptr<class CTexture> m_pDefaultNoise;
	shared_ptr<class CTexture> m_pDefaultRGBNoise;
	shared_ptr<class CTexture> m_pDefaultFlow;

	_bool m_bDebugMode = false;
	_uint m_iLevel = LEVEL_END;

public:
	void Free();
};

END