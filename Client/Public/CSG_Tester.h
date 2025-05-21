#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CDynamicCollider;
class CStaticCollider;
END


BEGIN(Client)



class CCSG_Tester : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;
	};

	struct FractureOptions 
	{
		int fractureCount = 10;
		_float3 vScale = { 1.f,1.f,1.f };
		_float3 vCenter = { 0.5f,0.5f,0.5f };
		int ClusterSize = 1;
		int WebNumAngle = 6;
		int WebNumLayer = 4;
		int  iMode = 0;
		bool bCrusterFracture = false;
		CModel::FRACTURE_MODE mode = CModel::FRACTURE_MODE::VORONOI;
	};

private:
	CCSG_Tester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCSG_Tester(const CCSG_Tester& rhs);
public:
	virtual ~CCSG_Tester();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	HRESULT Initialize_InstantFracture(shared_ptr<CModel> pModel, const FractureOptions& tOptions, _float fExplodeIntensity, shared_ptr<CTransform> pTransform);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Simulate(const PxSweepHit& Hit, const _float3& vDir);
	void End_Sim();

	void Bake_Binary();
	void Apply_Inner_Texture(const string& szFilename, shared_ptr<CTexture> pTexture, aiTextureType eType);

	void Dispatch_Fracture(const FractureOptions& tOptions);

	void Fracture(const FractureOptions& tOptions, vector<shared_ptr<MODEL_DATA>>& MDS, vector<vector<int>>& CellNeighbors);
	
private:
	void Fracture_Internal(const FractureOptions& tOptions, vector<shared_ptr<MODEL_DATA>>& MDS, vector<vector<int>>& CellNeighbors);

	void Save_HitInfo();
	HRESULT Render_Static();
	HRESULT Render_Dynamic();

	void PerformanceCounter();



private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CShader> m_pAnimShaderCom = { nullptr };
	shared_ptr<CModel> m_pCubeModelCom = { nullptr };
	shared_ptr<CModel> m_pSphereModelCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CStaticCollider> m_pStaticCollider = { nullptr };

	_float m_fSimTick = 0.f;
	_float m_fSimTotalTick = 0.f;
	const _float m_fSimMaxTick = 5.f;
	_uint m_iSimCount = 0;
	
	_bool m_bCrusterFracture = false;
	_float m_fFractureThreshold = 100.f;
	_bool m_bWorking = false;
	thread m_Worker;

	vector<_bool>					m_Detached;
	vector<_uint>					m_ParentCluster;
	map<_uint, vector<_uint>>		m_ChildCluster;
	vector<pair<PxVec3, PxVec3>>	m_PrevVelocity;

	shared_ptr<ANIMATION_DATA> m_pCurrAnimData;
	vector<shared_ptr<ANIMATION_DATA>> m_AnimationDatas;
	vector<HIT> m_vecHits;
	vector<PxRigidDynamic*> m_Actors;
	vector<_float3> m_Centers;
	_bool m_bHit = false;
	_wstring m_strModelTag;

	FractureOptions m_tFracOp;
	vector<shared_ptr<MODEL_DATA>> MDS;
	vector<vector<int>> CellNeighbors;
	_float m_fDebugTimer = 0.f;

	LARGE_INTEGER			m_CurTime;
	LARGE_INTEGER			m_OldTime;
	LARGE_INTEGER			m_OriginTime;
	LARGE_INTEGER			m_CpuTick;
	_float m_fTimeDelta = 0.f;

	_bool m_bInstantFracture = false;
	_float m_fExplodeIntensity = 0.f;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CCSG_Tester> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	static shared_ptr<CCSG_Tester> Create_Instant_Fracture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, shared_ptr<CModel> pStaticModel, 
		const FractureOptions& tOptions, _float fExplodeIntensity, shared_ptr<CTransform> pTransform);
	void Free();
};

END