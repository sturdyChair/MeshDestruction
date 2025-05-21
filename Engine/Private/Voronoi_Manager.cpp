#include "Voronoi_Manager.h"
#include "GameInstance.h"
#include "Math_Manager.h"
#include "hwiVoro/HV_Cell.h"
#include "hwiVoro/Container.h"
#include "hwiVoro/c_loop.h"
#include "hwiVoro/HV_Wall.h"
#include "Mesh.h"


CVoronoi_Manager::CVoronoi_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext },
	m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice); Safe_AddRef(m_pContext);
}

CVoronoi_Manager::~CVoronoi_Manager()
{
	Free();
}

HRESULT CVoronoi_Manager::Initialize()
{
	//TestFunction();
	return S_OK;
}

void CVoronoi_Manager::CellToMeshData(hwiVoro::CHV_Cell_Base& v, shared_ptr<MESH_DATA>& pMeshData, _float3 vOffset, const string& szName,_uint pid , const _float3& vScale, MESH_VTX_INFO& Info)
{
	vector<double> vert;
	v.vertices(vert);
	vector<_float3> vertices;
	for (int i = 0; i < vert.size(); i += 3)
	{
		vertices.push_back(_float3((vert[i] + vOffset.x) * vScale.x, (vert[i + 1] + vOffset.y) * vScale.y, (vert[i + 2] + vOffset.z) * vScale.z));
	}
	vector<double> norm;
	v.normals(norm);
	vector<_float3> normals(vertices.size(), { 0.f,0.f,0.f });
	vector<_float3> normalsFace;
	for (int i = 0; i < norm.size(); i += 3)
	{
		normalsFace.push_back(_float3(norm[i] * vScale.x, norm[i + 1] * vScale.y, norm[i + 2] * vScale.z));//might have to change z to -z
	}

	vector<_float2> texcoords(vertices.size(), { 0.5f,0.5f });


	vector<int> fo;
	v.face_orders(fo);
	vector<int> fv;
	v.face_vertices(fv);
	int idx = 0;
	vector<_uint> indices;
	int normIdx = 0;
	for (int faceSize : fo)
	{
		++idx;
		int nIdx = idx + faceSize;
		normals[fv[idx]].x += normalsFace[normIdx].x;
		normals[fv[idx]].y += normalsFace[normIdx].y;
		normals[fv[idx]].z += normalsFace[normIdx].z;

		normals[fv[idx + 1]].x += normalsFace[normIdx].x;
		normals[fv[idx + 1]].y += normalsFace[normIdx].y;
		normals[fv[idx + 1]].z += normalsFace[normIdx].z;
		for (int i = 1; i < faceSize - 1; ++i)
		{
			indices.push_back((_uint)fv[idx]);
			indices.push_back((_uint)fv[idx + i + 1]); //
			indices.push_back((_uint)fv[idx + i]);     //change this two lines order to change winding order
			normals[fv[idx + i + 1]].x += normalsFace[normIdx].x;
			normals[fv[idx + i + 1]].y += normalsFace[normIdx].y;
			normals[fv[idx + i + 1]].z += normalsFace[normIdx].z;
		}
		++normIdx;

		idx = nIdx;
	}

	pMeshData = make_shared<MESH_DATA>();

	pMeshData->Make_VCMesh(vertices, normals, indices, texcoords, szName, 1, pid, Info, XMMatrixIdentity());
}

void CVoronoi_Manager::TestFunction()
{
	//hwiVoro::container c{ -1, 1, -1, 1, -1, 1, 6, 6, 6,false,false,false, 8 };
	//hwiVoro::wall_cylinder wall(0, 0, 0, 0, 0, 1, 0.5);
	//hwiVoro::wall_sphere wall_s(0,0,0,0.4);
	//c.add_wall(wall_s);
	//for (int i = 0; i < 2000; ++i)
	//{
	//	double x = CMath_Manager::Random_Float(-0.4f, 0.4f);
	//	double y = CMath_Manager::Random_Float(-0.4f, 0.4f);
	//	double z = CMath_Manager::Random_Float(-0.4f, 0.4f);
	//	if (c.point_inside(x, y, z))
	//		c.put(i, x, y, z);
	//}
	//MESH_VTX_INFO Info;
	//hwiVoro::c_loop_all cl(c);
	//int idx = 0;
	//if (cl.start()) do
	//{
	//	hwiVoro::CHV_Cell tempCell;
	//	if (c.compute_cell(tempCell, cl))
	//	{
	//		
	//		shared_ptr<MESH_DATA> pMeshData;
	//		double x, y, z;
	//		cl.pos(x, y, z);
	//		c.apply_walls(tempCell, x, y, z);
	//		CellToMeshData(tempCell, pMeshData, _float3(x,y,z), string("Voronoi_") + to_string(cl.pid()), cl.pid(), Info);
	//		vector<shared_ptr<class CBone>> Bones;
	//		m_vecMesh.push_back(CMesh::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, pMeshData, Bones, XMMatrixIdentity()));
	//	}
	//	++idx;
	//} while (cl.inc());



}

void CVoronoi_Manager::Debug_Render()
{
	for (auto& mesh : m_vecMesh)
	{
 		//mesh->Render();
	}
}

shared_ptr<CVoronoi_Manager> CVoronoi_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CVoronoi_Manager
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVoronoi_Manager(pDevice, pContext)
		{
		}
	};
	shared_ptr<CVoronoi_Manager> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CVoronoi_Manager"));
		return nullptr;
	}

	return pInstance;
}

void CVoronoi_Manager::Free()
{

	Safe_Release(m_pDevice); Safe_Release(m_pContext);
}
