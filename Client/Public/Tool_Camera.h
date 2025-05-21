#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Tool : public CCamera
{
public:
	struct CAMERA_FREE_DESC : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = {};
	};

private:
	CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Tool(const CCamera_Tool& rhs);

public:
	~CCamera_Tool();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float m_fMouseSensor = {};

public:
	static shared_ptr<CCamera_Tool> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END