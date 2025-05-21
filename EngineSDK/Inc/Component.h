
#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
public:
	virtual ~CComponent();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() { return S_OK; }
	shared_ptr<class CGameObject> Get_Owner();
	void Set_Owner(shared_ptr<class CGameObject> pOwner);

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	weak_ptr<class CGameInstance> m_pGameInstance;
	_bool						m_isCloned = { false };
	weak_ptr<class CGameObject> m_pOwner;


public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	void Free();
};

END