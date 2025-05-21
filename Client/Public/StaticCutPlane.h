
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

class CStaticCutPlane : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CStaticCutPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticCutPlane(const CStaticCutPlane& rhs);

public:
	virtual ~CStaticCutPlane();

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
	shared_ptr<class CStaticCutTester> m_pCuttable;
	_bool m_bCutReady = false;
	_float m_fLocalRotation = 0.f;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CStaticCutPlane> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END