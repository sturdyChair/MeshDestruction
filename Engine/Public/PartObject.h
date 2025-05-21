#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	struct PARTOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		shared_ptr<CTransform>  pParentTransform;
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	shared_ptr<CTransform> m_pParentTransform = { nullptr };

	_float4x4					m_WorldMatrix = {};

protected:
	void SetUp_WorldMatrix(_fmatrix ChildWorldMatrix);

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	void Free();
};

END