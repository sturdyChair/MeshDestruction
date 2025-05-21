#pragma once

#include "PxCollider.h"

BEGIN(Engine)

class ENGINE_DLL CStaticCollider : public CPxCollider
{
private:
	CStaticCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticCollider(const CStaticCollider& rhs);
public:
	virtual ~CStaticCollider();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	void Remove_Actor();
	void Add_Actor();
	//bool Reshape();

private:
	PxRigidStatic* m_pRigidStatic = nullptr;
	_float4x4 m_PrevTransform;

public:
	static shared_ptr<CStaticCollider> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END