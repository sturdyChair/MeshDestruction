#pragma once
#include "Engine_Defines.h"


BEGIN(Engine)

class ENGINE_DLL COctree
{
public:
	struct ENGINE_DLL Node
	{
		enum SIDE 
		{
			N_N_N,
			N_N_P,
			P_N_P,
			P_N_N,
			N_P_N,
			N_P_P,
			P_P_P,
			P_P_N,
		};
		_int iTopology;
		_bool bInFrustum = true;
		_bool bInFull = true;
		_float3 Center;
		//_float3 Vertex[8];
		shared_ptr<Node> Children[8];
		_float fRadius;
		_float fOuterRadius;

		vector<shared_ptr<class COctComp>> vecObjects;

		void Initialize(_float fUnitScale, _int _iTopology, const _float3& vCenter)
		{
			
			Center = vCenter;
			iTopology = _iTopology;
			fRadius = fUnitScale * pow(2, iTopology);
			_float fHalfSize = fRadius * 0.5f;
			//Vertex[0] = _float3{ vCenter.x - fRadius, vCenter.y - fRadius, vCenter.z - fRadius };
			//Vertex[1] = _float3{ vCenter.x - fRadius, vCenter.y - fRadius, vCenter.z + fRadius };
			//Vertex[2] = _float3{ vCenter.x + fRadius, vCenter.y - fRadius, vCenter.z + fRadius };
			//Vertex[3] = _float3{ vCenter.x + fRadius, vCenter.y - fRadius, vCenter.z - fRadius };
			//Vertex[4] = _float3{ vCenter.x - fRadius, vCenter.y + fRadius, vCenter.z - fRadius };
			//Vertex[5] = _float3{ vCenter.x - fRadius, vCenter.y + fRadius, vCenter.z + fRadius };
			//Vertex[6] = _float3{ vCenter.x + fRadius, vCenter.y + fRadius, vCenter.z + fRadius };
			//Vertex[7] = _float3{ vCenter.x + fRadius, vCenter.y + fRadius, vCenter.z - fRadius };
			bInFrustum = true;
			fOuterRadius = sqrtf(powf(fRadius, 2.f) * 3.f );
			if (iTopology <= 0)
				return;
			if(!Children[0])
				Children[0] = make_shared<Node>();
			Children[0]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x - fHalfSize, vCenter.y - fHalfSize, vCenter.z - fHalfSize });
			if (!Children[1])
				Children[1] = make_shared<Node>();
			Children[1]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x - fHalfSize, vCenter.y - fHalfSize, vCenter.z + fHalfSize });
			if (!Children[2])
				Children[2] = make_shared<Node>();
			Children[2]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x + fHalfSize, vCenter.y - fHalfSize, vCenter.z + fHalfSize });
			if (!Children[3])
				Children[3] = make_shared<Node>();
			Children[3]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x + fHalfSize, vCenter.y - fHalfSize, vCenter.z - fHalfSize });
			if (!Children[4])
				Children[4] = make_shared<Node>();
			Children[4]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x - fHalfSize, vCenter.y + fHalfSize, vCenter.z - fHalfSize });
			if (!Children[5])
				Children[5] = make_shared<Node>();															 
			Children[5]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x - fHalfSize, vCenter.y + fHalfSize, vCenter.z + fHalfSize });
			if (!Children[6])
				Children[6] = make_shared<Node>();															 
			Children[6]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x + fHalfSize, vCenter.y + fHalfSize, vCenter.z + fHalfSize });
			if (!Children[7])
				Children[7] = make_shared<Node>();															 
			Children[7]->Initialize(fUnitScale, iTopology - 1, _float3{ vCenter.x + fHalfSize, vCenter.y + fHalfSize, vCenter.z - fHalfSize });
		}
		void Update(_bool _bInFrustum, _bool _bInFull);
		void Set_Up();
		_bool IsInOctree(const _float3& vCenter, _float _fRadius)
		{
			_float fDiffX = Center.x - vCenter.x;
			_float fDiffY = Center.y - vCenter.y;
			_float fDiffZ = Center.z - vCenter.z;
			_float fLen = _fRadius + fRadius;
			if (abs(fDiffX) > fLen)
			{
				return false;
			}
			if (abs(fDiffY) > fLen)
			{
				return false;
			}
			if (abs(fDiffZ) > fLen)
			{
				return false;
			}
			return true;
		}

		_bool IsInOctreeEx(const _float3& vCenter, _float _fRadius)
		{
			_float fDiffX = Center.x - vCenter.x;
			_float fDiffY = Center.y - vCenter.y;
			_float fDiffZ = Center.z - vCenter.z;
			_float fLen = _fRadius - fRadius;
			if (abs(fDiffX) > fLen)
			{
				return false;
			}
			if (abs(fDiffY) > fLen)
			{
				return false;
			}
			if (abs(fDiffZ) > fLen)
			{
				return false;
			}
			return true;
		}
		
		_bool AddOctComp(shared_ptr<COctComp> pComp);


		void Clear()
		{
			vecObjects.clear();
	
			for (_uint i = 0; i < 8; ++i)
			{
				if(Children[i])
					Children[i]->Clear();
			}

		}
	};
private:
	COctree();
public:
	virtual ~COctree();

public:
	HRESULT Initialize(_float fUnitScale, _int iMaxRecursion, const _float3& vCenter);//일단은 iMaxRecursion에 6 이상 넣지 마세요
	void Set_Up();
	void Update();
	shared_ptr<Node> Get_Node() { return m_pOctree; }
	void Clear();
	void Start();
	void Stop();

private:
	weak_ptr<class CGameInstance> m_pGameInstance;

	_float m_fUnitScale = 0.f;
	_int m_iMaxRecursion = 10;
	
	_float3 m_vCenter = { 0.f,0.f,0.f };

	shared_ptr<Node> m_pOctree;
	_bool m_bCull = false;

public:
	static shared_ptr<COctree> Create();
	void Free();
};

END