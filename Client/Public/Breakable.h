#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"
#include "BlastManager.h"
#include "NvBlastExtAuthoringMesh.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CMesh;
class CDynamicCollider;
class CStaticCollider;
END


BEGIN(Client)



class CBreakable : public CGameObject
{
public:
	static wstring ObjID;
	struct FDO_DESC : public CTransform::TRANSFORM_DESC
	{
		_wstring strModelTag;
		_float fTorqueThreshold = 1000.f;
		_float fForceThreshold = 1000.f;
		_uint iNumCell = 200;
		_wstring strInD;
		_wstring strInN;
	};

	struct ChunkNode
	{
		_bool isLeaf = false;
		_bool isRoot = false;
		_uint MeshIdx = 0;
		int parentId = -1;
		int chunkId = -1;
		_float3 maximum{0.f,0.f,0.f};
		_float3 minimum{ 0.f,0.f,0.f};
		_float3 center{0.f,0.f,0.f};
		_float4x4 Transform = Identity;
		weak_ptr<ChunkNode> pParent;
		set<shared_ptr<ChunkNode>> pChildren;
		void Initialize(const Nv::Blast::ChunkInfo& Info);
		_bool Intersect(const _float3& pos, _float fRadius = 1.f) const;
		vector<_uint> Get_Shapes() const;
		//PxRigidDynamic* pActor = nullptr;
	};

	struct chunkEdge
	{
		_float fHealth = 0.f;
		_float3 vCenter{0.f,0.f,0.f};
		_int iActorIdx = 0;
	};

	struct Actor
	{
		PxRigidDynamic* pRigidBody = nullptr;
		vector<_uint> Nodes;
		_float4x4	m_Transform;
	};

private:
	CBreakable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBreakable(const CBreakable& rhs);
public:
	virtual ~CBreakable();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnPxContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPair, CGameObject* pOther) override;

	void Apply_Damage_Shape(const PxGeometry& geo ,const PxTransform& pose);

	void Apply_Inner_Texture(const string& szFilename, shared_ptr<CTexture> pTexture, aiTextureType eType);

private:
	HRESULT Render_Static();

	void PerformanceCounter();

	void Fracture(_uint iMaxChild, _uint iMaxDepth);
	void Fracture_Root(_uint iMaxChild, _uint iMaxDepth);

	void Build_Hierarchy();
	void Insert_Nodes();

	void CheckIsland();

	void DamageFunction(PxContactPairPoint& pp, _int iActor0Idx, _int iActor1Idx);

	shared_ptr<MESH_DATA> Build_Mesh(_uint i);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };//For Root
	vector<shared_ptr<CMesh>> m_Meshes = { nullptr };
	vector<PxShape*> m_Shapes;
	vector<_uint>   m_OwnerActor;
	
	shared_ptr<MODEL_DATA> m_pModelData = { nullptr };

	vector<map<_uint, chunkEdge>> m_Edges;
	vector<Actor> m_Actors;
	

	_wstring m_strModelTag = L"";
	_float m_fForceThreshold = 1000.f;
	_float m_fTorqueThreshold = 1000.f;
	_uint m_iNumCell = 200;
	
	_bool m_bDirty = false;
	_bool m_bOnContact = true;
	_bool m_bContact = true;
	_int m_iCount = 1;

	shared_ptr<CDynamicCollider> m_pDynamicColliderCom = { nullptr };

	int32_t iNumBond = 0;
	
	_bool m_bIsRoot = false;
	_bool m_bIsLeaf = false;


	RandomGenerator randomGen;
	Nv::Blast::FractureTool* m_pFractureTool = nullptr;
	map<int, shared_ptr<ChunkNode>> m_Nodes;
	
	_wstring m_strInD;
	_wstring m_strInN;

	LARGE_INTEGER			m_CurTime;
	LARGE_INTEGER			m_OldTime;
	LARGE_INTEGER			m_OriginTime;
	LARGE_INTEGER			m_CpuTick;
	_float m_fTimeDelta = 0.f;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CBreakable> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	void Free();
};

END