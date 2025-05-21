#pragma once

#include "PxCollider.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicCollider : public CPxCollider
{
private:
	CDynamicCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamicCollider(const CDynamicCollider& rhs);
public:
	virtual ~CDynamicCollider();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	PxRigidDynamic* Get_RigidBody() { return m_pRigidDynamic; }
	_bool Is_Kinematic();
private:
	PxRigidDynamic* m_pRigidDynamic = nullptr;
	_float4x4 m_PrevTransform;
	_float3 m_vScale;

public:
	static shared_ptr<CDynamicCollider> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END