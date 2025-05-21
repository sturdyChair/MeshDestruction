#pragma once
#include "Engine_Defines.h"
#include "NvBlastExtAuthoring.h"
#include "NvBlastExtAuthoringFractureTool.h"
#include "Math_Manager.h"

using BlastMesh = Nv::Blast::Mesh;

class RandomGenerator : public Nv::Blast::RandomGeneratorBase
{
public:
	virtual float getRandomValue()override { return CMath_Manager::Random_Float(0.f, 1.f); };
	virtual void seed(int32_t seed) override {};
};


BEGIN(Engine)

class ENGINE_DLL CBlastManager
{
	//DECLARE_SINGLETON(CBlastManager)

private:
	CBlastManager();
public:
	virtual ~CBlastManager();

	void FreeBlastMesh(BlastMesh*& pMesh);
	void CleanseMesh(BlastMesh*& pMesh);
	BlastMesh* ConvertMeshData(const MESH_DATA& meshData, _float fMaterialIdx = 0.f);
	//shared_ptr<MESH_DATA> RevertMeshData(const BlastMesh* pMesh, _bool ToAnim, _uint iBlendIdx = 0);
	vector<shared_ptr<MESH_DATA>> Voronoi_Fracture(BlastMesh* pMesh, _uint iNumCell, _bool bAnim = true);



private:
	//vector<shared_ptr<MESH_DATA>> RevertMeshData();
	RandomGenerator randomGen;

	Nv::Blast::FractureTool* m_pFractureTool = nullptr;
	Nv::Blast::MeshCleaner* m_pMeshCleaner = nullptr;
public:
	void Free();
	static shared_ptr<CBlastManager> Create();
};

END