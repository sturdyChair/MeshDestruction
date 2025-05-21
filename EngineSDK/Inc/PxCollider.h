#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CPxCollider : public CComponent
{
public:
	struct PXCOLLIDER_DESC
	{
		_float fStaticFriction = 0.5f;
		_float fDynamicFriction = 0.5f;
		_float fRestitution = 0.5f;
		CGameObject* pOwner = nullptr;
		wstring strModelComTag = L"";
	};


protected:
	CPxCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPxCollider(const CPxCollider& rhs);
public:
	virtual ~CPxCollider();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Update(_float fTimeDelta);

protected:
	void Attach_Shape(PxRigidActor* pActor, PxMaterial* mat, PxVec3 vScale);

protected:
	shared_ptr<class CTransform> m_pTransform = nullptr;
	shared_ptr<class CModel>     m_pModel = nullptr;
	PxPhysics* m_pPhysics = nullptr;
	PxScene* m_pCurrentScene = nullptr;
	
public:
	static shared_ptr<CPxCollider> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END