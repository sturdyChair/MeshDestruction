#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_BeatMarker : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_BeatMarkerInfo : public CUi_2DInfo
	{
		_bool bRight = { true };
	};

protected:
	CUi_BeatMarker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_BeatMarker(const CUi_2D& rhs);


public:
	virtual ~CUi_BeatMarker();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


public:
	_bool Get_Right() const { return m_bRight; }


private:
	_bool m_bRight = { true };


public:
	static shared_ptr<CUi_BeatMarker> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
