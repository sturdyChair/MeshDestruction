#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CLight : public CGameObject
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CLight();

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	//HRESULT Initialize(const LIGHT_DESC& LightDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_LightState(_bool bIsOn) { m_bIsOn = bIsOn; }
	_bool Get_LightState() { return m_bIsOn; }

	void Set_LightDiffuse(_float4 vDiffuse) { m_LightDesc.vDiffuse = vDiffuse; }
	_float4 Get_LightDiffuse() { return m_LightDesc.vDiffuse; }
	_float* Get_LightDiffusePtr() { return &m_LightDesc.vDiffuse.x; }

	void Set_LightSpecular(_float4 vSpecular) { m_LightDesc.vSpecular = vSpecular; }
	_float4 Get_LightSpecular() { return m_LightDesc.vSpecular; }
	_float* Get_LightSpecularPtr() { return &m_LightDesc.vSpecular.x; }

	void Set_LightAmbient(_float4 vAmbient) { m_LightDesc.vAmbient = vAmbient; }
	_float4 Get_LightAmbient() { return m_LightDesc.vAmbient; }
	_float* Get_LightAmbientPtr() { return &m_LightDesc.vAmbient.x; }

	void Set_LightRange(_float fRange) { m_LightDesc.fRange = fRange; }
	_float Get_LightRange() { return m_LightDesc.fRange; }
	_float* Get_LightRangePtr() { return &m_LightDesc.fRange; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool m_bIsOn = true;
	//_bool m_bIsBlink = false;

private:
	LIGHT_DESC			m_LightDesc = {};

	shared_ptr<class CShader> m_pShaderCom = { nullptr };
	shared_ptr<class CModel> m_pModelCom = { nullptr };

	_float m_fTimeAcc = 0.f;
	_uint  m_iPattern = 0;
public:
	static shared_ptr<CLight> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* LightDesc);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override { return nullptr; };
	void Free();
};

END