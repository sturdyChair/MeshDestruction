#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CCollider;
END


BEGIN(Client)

class CBarrier : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CBarrier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBarrier(const CBarrier& rhs);

public:
	virtual ~CBarrier();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_ParentTransform(const _float4x4& Parent) { m_matParent = Parent; }


public:
	virtual void EffectStartCallBack();
	virtual void EffectEndCallBack();

	virtual void EffectSoftEnd();
	virtual void EffectHardEnd();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };
	shared_ptr<CCollider> m_pCollider = { nullptr };
	_float4x4 m_matParent;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CBarrier> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END