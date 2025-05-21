#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public enable_shared_from_this<CLevel>
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CLevel();

public:
	_uint Get_LevelID() const {
		return m_iLevelID;
	}

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	weak_ptr<class CGameInstance> m_pGameInstance;
	_uint						m_iLevelID = { 0 };

public:
	void Free();
};

END