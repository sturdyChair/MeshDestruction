#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CMesh;
class CDynamicCollider;
class CStaticCollider;
class CStatic_Cutter;
class CCharacterController;
END


BEGIN(Client)
class CCuttable;


class CDynamicFracture : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_uint iNumCell = 100;
		_wstring strModelTag;
		_wstring strInD;
		_wstring strInN;
	};
	struct Fragment
	{
		shared_ptr<CMesh> pMesh;
		_float4x4 Transform = Identity;
		PxRigidDynamic* pActor = nullptr;
		_uint iBindBoneIdx = 0;
	};

private:
	CDynamicFracture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamicFracture(const CDynamicFracture& rhs);
public:
	virtual ~CDynamicFracture();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnPxContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPair, CGameObject* pOther) override;

	void Set_Position(const _float3& vPos);

private:
	HRESULT Render_Static();
	HRESULT Render_Dynamic();

	void Snap_Mesh();

	void PreFracture(_uint iNumCell);

	void AddCollider(const string& szBoneName, _float fRadius, _float fHeight);
	void UpdateFragments();
	void ActivateFragments();
	void BindFragments();
	_uint FindBestMatchingBone(PxConvexMeshGeometry& geo);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CShader> m_pStaticShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CModel> m_pFracturedModelCom = { nullptr };
	_bool m_bFracture = false;
	_float4x4 m_PreTransform;
	
	_wstring m_strInD;
	_wstring m_strInN;

	_bool m_bSnape = false;
	

	shared_ptr<CCuttable> m_pCuttable = { nullptr };

	_wstring m_strModelTag;
	_float m_LifeTimer = 0.f;
	_float m_fMaxLife = 5.f;

	vector<shared_ptr<MODEL_DATA>> m_PreFracturedData;
	vector<Fragment> m_Fragments;
	
	
	vector<pair<PxCapsuleGeometry, PxTransform>> m_Colliders;
	vector<string>			m_ColliderBones;
	vector<_float4x4>		m_ColliderBoneOriginals;

	shared_ptr< CCharacterController> m_pCCT;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CDynamicFracture> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	shared_ptr<CDynamicFracture> ReClone();

	void Free();
};

END