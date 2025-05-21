#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CImpact_Spark_Effect : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CImpact_Spark_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CImpact_Spark_Effect(const CImpact_Spark_Effect& rhs);

public:
	virtual ~CImpact_Spark_Effect();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void EffectStartCallBack();
	virtual void EffectEndCallBack();

	virtual void EffectSoftEnd();
	virtual void EffectHardEnd();

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CTexture>		m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };
	shared_ptr<CTexture>		m_pNoiseTextureCom = { nullptr };
	_uint m_iTexIdx = 0;
	_float3 m_vDir;
	_float  m_fSize = 1.f;
	_float4x4 m_Local;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CImpact_Spark_Effect> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END