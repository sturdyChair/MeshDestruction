#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CDynamicCollider;
class CStaticCollider;
class CCutter_Controller;
class CMultiCutterController;
class CPxRagDoll;
class CCollider;
END


BEGIN(Client)



class CWoundable : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;
	};


private:
	CWoundable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWoundable(const CWoundable& rhs);
public:
	virtual ~CWoundable();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	shared_ptr<CTransform> Get_WoundTransform(_uint idx) { return m_pWoundTransform[idx]; }

	virtual void TakeDamage(const DamageInfo& Info) override;
	virtual void Sweeped(const PxSweepHit& hit, const DamageInfo& damageInfo) override;

private:
	void AddCollider(const string& szBoneName, _float fRadius, _float fHeight);
	void UpdateColliders();

	void Render_Sphere();
	vector<_uint> FindBestMatchingBone(PxConvexMeshGeometry& geo, PxTransform& pose);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CShader> m_pSphereShaderCom = { nullptr };
	
	shared_ptr<CTransform> m_pWoundTransform[2];
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CModel> m_pSphereModelCom = { nullptr };
	vector<PxConvexMeshGeometry> m_SphereGeos;
	shared_ptr<CCollider> m_pColliderCom;
	shared_ptr<CCollider> m_pColliderCom2;
	_float4x4 m_Ellipsoid[2];
	_float4x4 m_EllipsoidInv[2];
	const _float4x4* m_pBoneMatrix[2] = {};
	const _float4x4* m_pOriBoneMatrix[2] = {};
	_float4x4 m_PreTransform;
	
	_float4x4 m_WoundTransform[2];
	_wstring m_strModelTag;
	_float m_fTimer = 0.f;
	_uint m_iWoundCount = 0;

	vector<vector<_uint>> m_BestMatchingBoneIdx[2];

	vector<PxRigidDynamic*> m_Colliders;
	vector<pair<PxCapsuleGeometry, PxTransform>> m_ColliderGeometrys;
	vector<string>			m_ColliderBones;
	vector<_float4x4>		m_ColliderBoneOriginals;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CWoundable> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END