
#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CCharacterController;
END

BEGIN(Client)

class CSentientGopro : public CCamera
{
public:
	static wstring ObjID;
	struct GOPRO_DESC : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = {};
	};

private:
	CSentientGopro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSentientGopro(const CSentientGopro& rhs);

public:
	~CSentientGopro();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();

private:

	_float3 m_vOffset = { 0.f,2.f,0.f };

	_float m_fMomentum = 0.f;
	_float m_fMouseSensor = {};
	shared_ptr< CCharacterController> m_pCCT;

public:
	static shared_ptr<CSentientGopro> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END