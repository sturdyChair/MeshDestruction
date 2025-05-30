#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Base.h"
#include "VIBuffer_Instance.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CMyParticle : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CMyParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMyParticle(const CMyParticle& rhs);

public:
	virtual ~CMyParticle();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Push_From_Pool(void* pArg)override;
	virtual void Dying() override;

public:
	virtual void EffectStartCallBack();
	virtual void EffectEndCallBack();

	virtual void EffectSoftEnd();
	virtual void EffectHardEnd();

public:
	_float3 Get_Pivot();
	void Set_Pivot(_float3 vPivot);

	_float2 Get_Speed();
	void Set_Speed(_float2 vSpeed);

	_float3 Get_Range();
	void Set_Range(_float3 vRange);

	_bool Get_IsRandomize();
	void Set_IsRandomize(_bool bIsRandomize);

	_float2 Get_Scale();
	void Set_Scale(_float2 vScale);

	CVIBuffer_Instance::INSTANCE_STYLE Get_Style();
	void Set_Style(CVIBuffer_Instance::INSTANCE_STYLE eStyle);

private:
	shared_ptr<CShader>									m_pShaderCom = { nullptr };
	shared_ptr<CTexture>								m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Point_Instance>				m_pVIBufferCom = { nullptr };
	shared_ptr<CTexture>								m_pNoiseTextureCom = { nullptr };

	_float3 m_vDir;
	_float m_fLifeTime;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CMyParticle> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END