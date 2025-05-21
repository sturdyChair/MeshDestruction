#pragma once

#include "Component.h"

BEGIN(Engine)

using PxArticulation = PxArticulationReducedCoordinate;

class ENGINE_DLL CPxRagDoll : public CComponent
{
public:
	struct PXRAGDOLL_DESC
	{
		_float fStaticFriction = 0.5f;
		_float fDynamicFriction = 0.4f;
		_float fRestitution = 0.5f;
		shared_ptr<class CModel> pModel;
		shared_ptr<class CTransform> pTransform;
	};


private:
	CPxRagDoll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPxRagDoll(const CPxRagDoll& rhs);
public:
	virtual ~CPxRagDoll();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Update(_float fTimeDelta);
	
	void Slice(const _float4& plane, _bool bReverse = false);
	void Slice(_fvector plane, _bool bReverse = false);
	void Slice(const vector<_float4>& planes, _bool bReverse = false);

	void Build_Ragdoll(const PXRAGDOLL_DESC& desc, const vector<_bool>& detache = {});
	void Build_Ragdoll_With_Validity(const PXRAGDOLL_DESC& desc, const vector<_bool>& validBones);
	void Activate();
	void Deactivate();
	_bool isActive()const { return m_bActive; }
	void Purge_Nodes();
	vector<_bool> Get_Detached() const;

	void Set_Velocity(_uint iArticulationIdx, const _float3& vVelocity);
	void Set_Velocity(_uint iArticulationIdx, const _float4& vVelocity);
	void Set_Velocity(_uint iArticulationIdx, _fvector vVelocity);
	void Set_Velocity(_uint iArticulationIdx, const PxVec3& vVelocity);

	void ApplyBones();

private:
	void Rebuild();

private:
	shared_ptr<class CTransform> m_pTransform = nullptr;
	shared_ptr<class CModel>     m_pModel = nullptr;
	PxPhysics* m_pPhysics = nullptr;
	PxScene* m_pCurrentScene = nullptr;
	_bool m_bActive = false;
	
	_uint m_iRootIdx = 0;
	vector<_uint> m_Roots;
	vector<vector<_uint>> m_Children;
	vector<PxArticulationLink*> m_Nodes;
	vector<_float3> m_Scales;

	vector<PxArticulation*> m_Articulations;

	_float m_fSFriction = 0.f;
	_float m_fDFriction = 0.f;
	_float m_fRestitution = 0.f;

public:
	static shared_ptr<CPxRagDoll> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END