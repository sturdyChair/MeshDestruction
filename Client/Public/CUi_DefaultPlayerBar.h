#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_DefaultPlayerBar : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_PlayerBarInfo : public CUi_2DInfo
	{
		_int iTextureNum;
	};

protected:
	CUi_DefaultPlayerBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_DefaultPlayerBar(const CUi_2D& rhs);


public:
	virtual ~CUi_DefaultPlayerBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	void Initialize_PlayerBar();
	_uint m_iTextureNum = { 0 };


public:
	static shared_ptr<CUi_DefaultPlayerBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
