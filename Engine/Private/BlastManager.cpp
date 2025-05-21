#include "BlastManager.h"
#include "NvBlastExtAuthoringMesh.h"
#include "Timer.h"
#include "NvBlastExtAuthoringMeshCleaner.h"
#include <iostream>

//IMPLEMENT_SINGLETON(CBlastManager)

CBlastManager::CBlastManager()
{
}

CBlastManager::~CBlastManager()
{
	Free();
}

void CBlastManager::FreeBlastMesh(BlastMesh*& pMesh)
{
	if (pMesh)
	{
		pMesh->release();
		pMesh = nullptr;
	}
}

void CBlastManager::CleanseMesh(BlastMesh*& pMesh)
{
	if (!m_pMeshCleaner)
	{
		m_pMeshCleaner = NvBlastExtAuthoringCreateMeshCleaner();
		if (!m_pMeshCleaner)
		{
			cout << "Failed to create MeshCleaner\n";
			return;
		}
	}
	if (!pMesh)
	{
		cout << "Invalid BlastMesh\n";
		return;
	}
	auto rtMesh = m_pMeshCleaner->cleanMesh(pMesh);
	if (rtMesh)
	{
		pMesh->release();
		pMesh = rtMesh;
	}
	else
	{
		cout << "Failed to clean mesh\n";
	}
}

BlastMesh* CBlastManager::ConvertMeshData(const MESH_DATA& meshData, _float fMaterialIdx)
{
	NvcVec3* positions = new NvcVec3[meshData.iNumVertices];
	NvcVec3* normals = new NvcVec3[meshData.iNumVertices];
	NvcVec2* uv = new NvcVec2[meshData.iNumVertices];
	uint32_t numVertices = meshData.iNumVertices;
	uint32_t* indices = new uint32_t[meshData.iNumFaces * 3];
	uint32_t numIndices = meshData.iNumFaces * 3;


	for (uint32_t i = 0; i < numVertices; ++i)
	{
		memcpy(&positions[i], &meshData.pVertices[i].vPosition, sizeof(NvcVec3));
		memcpy(&normals[i], &meshData.pVertices[i].vNormal, sizeof(NvcVec3));
		memcpy(&uv[i], &meshData.pVertices[i].vTexcoord, sizeof(NvcVec2));
	}
	for (uint32_t i = 0; i < meshData.iNumFaces; ++i)
	{
		indices[i * 3] = meshData.pIndices[i]._1;
		indices[i * 3 + 1] = meshData.pIndices[i]._2;
		indices[i * 3 + 2] = meshData.pIndices[i]._3;
	}

	BlastMesh* pMesh = NvBlastExtAuthoringCreateMesh(positions, normals, uv, numVertices, indices, numIndices);
	delete[] positions;
	delete[] normals;
	delete[] uv;
	delete[] indices;
	if (!pMesh)
	{
		cout << "Failed to create BlastMesh\n";
	}

	return pMesh;
}

//shared_ptr<MESH_DATA> CBlastManager::RevertMeshData(const BlastMesh* pMesh, _bool ToAnim, _uint iBlendIdx)
//{
//	if (!pMesh)
//	{
//		cout << "Invalid BlastMesh\n";
//		return shared_ptr<MESH_DATA>();
//	}
//	if (pMesh->getVerticesCount() == 0 || pMesh->getFacetCount() == 0)
//	{
//		cout << "Invalid BlastMesh data\n";
//		return shared_ptr<MESH_DATA>();
//	}
//	shared_ptr<MESH_DATA> pMeshData = make_shared<MESH_DATA>();
//	pMeshData->iNumVertices = pMesh->getVerticesCount();
//	pMeshData->iNumFaces = pMesh->getFacetCount();
//	pMeshData->pVertices = shared_ptr<VTXMESH[]>( new VTXMESH[pMeshData->iNumVertices]);
//	pMeshData->pIndices = shared_ptr<FACEINDICES32[]>(new FACEINDICES32[pMeshData->iNumFaces]);
//	const Nv::Blast::Vertex* vertices = pMesh->getVertices();
//	const Nv::Blast::Facet* Facets = pMesh->getFacetsBuffer();
//	const Nv::Blast::Edge* edges = pMesh->getEdges();
//
//	for (uint32_t i = 0; i < pMeshData->iNumVertices; ++i)
//	{
//		memcpy(&pMeshData->pVertices[i].vPosition, &vertices[i].p, sizeof(NvcVec3));
//		memcpy(&pMeshData->pVertices[i].vNormal, &vertices[i].n, sizeof(NvcVec3));
//		memcpy(&pMeshData->pVertices[i].vTexcoord, &vertices[i].uv, sizeof(NvcVec2));
//	}
//	for (uint32_t i = 0; i < pMeshData->iNumFaces; ++i)
//	{
//		pMeshData->pIndices[i]._1 = Facets[i].firstEdgeNumber;
//	}
//
//	return pMeshData;
//}

vector<shared_ptr<MESH_DATA>> CBlastManager::Voronoi_Fracture(BlastMesh* pMesh, _uint iNumCell, _bool bAnim)
{
	if (!m_pFractureTool)
	{
		m_pFractureTool = NvBlastExtAuthoringCreateFractureTool();
		if (!m_pFractureTool)
		{
			cout << "Failed to create FractureTool\n";
			return {};
		}
		m_pFractureTool->setInteriorMaterialId(1);
		//m_pFractureTool->setRemoveIslands(true);
	}

	if (!pMesh)
	{
		cout << "Invalid BlastMesh\n";
		return {};
	}
	
	//if (m_pFractureTool->isMeshContainOpenEdges(pMesh))
	//{
	//	if(!m_pMeshCleaner)
	//		m_pMeshCleaner = NvBlastExtAuthoringCreateMeshCleaner();
	//	auto newMesh = m_pMeshCleaner->cleanMesh(pMesh);
	//	if (newMesh)
	//	{
	//		pMesh->release();
	//		pMesh = newMesh;
	//	}
	//
	//}


	Nv::Blast::VoronoiSitesGenerator* voronoi = NvBlastExtAuthoringCreateVoronoiSitesGenerator(pMesh, (&randomGen));
	if (!voronoi)
	{
		cout << "Failed to create VoronoiSitesGenerator\n";
		return {};
	}
	//auto pTimer = CTimer::Create();

	//pTimer->Update();
	voronoi->uniformlyGenerateSitesInMesh(iNumCell);
	const NvcVec3* sites = nullptr;
	uint32_t numSite = voronoi->getVoronoiSites(sites);
	m_pFractureTool->setSourceMeshes(&pMesh, 1);
	m_pFractureTool->voronoiFracturing(0, numSite, sites, false);
	m_pFractureTool->finalizeFracturing();
	//pTimer->Update();
	//cout << "Fracture : " << pTimer->Get_TimeDelta() << '\n';
	uint32_t numChunk = m_pFractureTool->getChunkCount();
	vector<shared_ptr<MESH_DATA>> returnMesh;
	m_pFractureTool->setRemoveIslands(true);
	returnMesh.reserve(numChunk);
	for (uint32_t i = 0; i < numChunk; ++i)
	{
		if(m_pFractureTool->getChunkDepth(i) <= 0)
		{
			continue;
		}
		Nv::Blast::Triangle* triangles = nullptr;
		
		uint32_t numTri = m_pFractureTool->getBaseMesh(i, triangles);
		shared_ptr<MESH_DATA> pMeshData = make_shared<MESH_DATA>();
		pMeshData->iNumVertices = numTri * 3;
		pMeshData->iNumFaces = numTri;
		if(bAnim)
			pMeshData->pAnimVertices = shared_ptr<VTXANIM[]>(new VTXANIM[pMeshData->iNumVertices]);
		else
			pMeshData->pVertices = shared_ptr<VTXMESH[]>(new VTXMESH[pMeshData->iNumVertices]);
		pMeshData->pIndices = shared_ptr<FACEINDICES32[]>(new FACEINDICES32[pMeshData->iNumFaces]);
		pMeshData->iMaterialIndex = 0;
		pMeshData->eModelType = bAnim ? MODEL_TYPE::ANIM : MODEL_TYPE::NONANIM;
		pMeshData->iNumBones = 0;
		pMeshData->Bone_Datas.clear();
		pMeshData->szName = "";
		if (triangles)
		{
			for (uint32_t j = 0; j < numTri; ++j)
			{
				if(bAnim)
				{
					memcpy(&pMeshData->pAnimVertices[j * 3].vPosition, &triangles[j].a.p, sizeof(_float3));
					memcpy(&pMeshData->pAnimVertices[j * 3].vNormal, &triangles[j].a.n, sizeof(_float3));
					memcpy(&pMeshData->pAnimVertices[j * 3].vTexcoord, &triangles[j].a.uv[0], sizeof(_float2));

					memcpy(&pMeshData->pAnimVertices[j * 3 + 1].vPosition, &triangles[j].b.p, sizeof(_float3));
					memcpy(&pMeshData->pAnimVertices[j * 3 + 1].vNormal, &triangles[j].b.n, sizeof(_float3));
					memcpy(&pMeshData->pAnimVertices[j * 3 + 1].vTexcoord, &triangles[j].b.uv[0], sizeof(_float2));

					memcpy(&pMeshData->pAnimVertices[j * 3 + 2].vPosition, &triangles[j].c.p, sizeof(_float3));
					memcpy(&pMeshData->pAnimVertices[j * 3 + 2].vNormal, &triangles[j].c.n, sizeof(_float3));
					memcpy(&pMeshData->pAnimVertices[j * 3 + 2].vTexcoord, &triangles[j].c.uv[0], sizeof(_float2));
					if (triangles[j].materialId == m_pFractureTool->getInteriorMaterialId())
					{
						pMeshData->pAnimVertices[j * 3].vTexcoord.x += 1.f;
						pMeshData->pAnimVertices[j * 3 + 1].vTexcoord.x += 1.f;
						pMeshData->pAnimVertices[j * 3 + 2].vTexcoord.x += 1.f;
						pMeshData->pAnimVertices[j * 3].vTexcoord.y += 1.f;
						pMeshData->pAnimVertices[j * 3 + 1].vTexcoord.y += 1.f;
						pMeshData->pAnimVertices[j * 3 + 2].vTexcoord.y += 1.f;

						pMeshData->pAnimVertices[j * 3].vBlendWeight.w = 1.f;
						pMeshData->pAnimVertices[j * 3 + 1].vBlendWeight.w = 1.f;
						pMeshData->pAnimVertices[j * 3 + 2].vBlendWeight.w = 1.f;
					}
				}
				else
				{
					memcpy(&pMeshData->pVertices[j * 3].vPosition, &triangles[j].a.p, sizeof(_float3));
					memcpy(&pMeshData->pVertices[j * 3].vNormal, &triangles[j].a.n, sizeof(_float3));
					memcpy(&pMeshData->pVertices[j * 3].vTexcoord, &triangles[j].a.uv[0], sizeof(_float2));

					memcpy(&pMeshData->pVertices[j * 3 + 1].vPosition, &triangles[j].b.p, sizeof(_float3));
					memcpy(&pMeshData->pVertices[j * 3 + 1].vNormal, &triangles[j].b.n, sizeof(_float3));
					memcpy(&pMeshData->pVertices[j * 3 + 1].vTexcoord, &triangles[j].b.uv[0], sizeof(_float2));

					memcpy(&pMeshData->pVertices[j * 3 + 2].vPosition, &triangles[j].c.p, sizeof(_float3));
					memcpy(&pMeshData->pVertices[j * 3 + 2].vNormal, &triangles[j].c.n, sizeof(_float3));
					memcpy(&pMeshData->pVertices[j * 3 + 2].vTexcoord, &triangles[j].c.uv[0], sizeof(_float2));
					if (triangles[j].materialId == m_pFractureTool->getInteriorMaterialId())
					{
						pMeshData->pVertices[j * 3].vTexcoord.x += 1.f;
						pMeshData->pVertices[j * 3 + 1].vTexcoord.x += 1.f;
						pMeshData->pVertices[j * 3 + 2].vTexcoord.x += 1.f;
						pMeshData->pVertices[j * 3].vTexcoord.y += 1.f;
						pMeshData->pVertices[j * 3 + 1].vTexcoord.y += 1.f;
						pMeshData->pVertices[j * 3 + 2].vTexcoord.y += 1.f;
					}
				}
				pMeshData->pIndices[j]._1 = j * 3;
				pMeshData->pIndices[j]._2 = j * 3 + 1;
				pMeshData->pIndices[j]._3 = j * 3 + 2;
				pMeshData->pIndices[j].numIndices = 3;
				
			}
			returnMesh.push_back(pMeshData);
		}
	}
	//pTimer->Update();
	//cout << "Make Mesh Data : " << pTimer->Get_TimeDelta() << '\n';
	delete voronoi;

	return returnMesh;
}

void CBlastManager::Free()
{
	if (m_pFractureTool)
	{
		m_pFractureTool->release();
		m_pFractureTool = nullptr;
	}
	if (m_pMeshCleaner)
	{
		m_pMeshCleaner->release();
		m_pMeshCleaner = nullptr;
	}

}

shared_ptr<CBlastManager> CBlastManager::Create()
{
	MAKE_SHARED_ENABLER(CBlastManager);
	shared_ptr<CBlastManager> pInstance = make_shared<MakeSharedEnabler>();

	return pInstance;

}
