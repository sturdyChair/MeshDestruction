#pragma once
#include "Engine_Defines.h"
#include "CSG_Data.h"
#include "NvBlastExtAuthoring.h"

BEGIN(Engine)
class ENGINE_DLL CCSG_Manager
{
public:
	static PlaneSide ClassifyVertex(const DOUBLE3& vertex, const CSG_Plane& plane, double fEpsModi = 1.f);
	static PlaneSide ClassifyTriangle(const CSG_Mesh& mesh, const _uint3& tris, const CSG_Plane& plane);
	static PlaneSide ClassifyTriangleAgainstPlane(const DOUBLE3& v0, const DOUBLE3& v1, const DOUBLE3& v2, const CSG_Plane& plane);
	static unique_ptr<BSP_Node> BuildBSPTree(CSG_Mesh& mesh, const vector<_uint3>& triangles, const _float3& center);
	static void AppendBSPTree(unique_ptr<BSP_Node>& node, CSG_Mesh& mesh);
	static unique_ptr<BSP_Node>& InvertBSPTree(unique_ptr<BSP_Node>& node);
	static unique_ptr<BSP_Node>& RevertBSPTree(unique_ptr<BSP_Node>& node);
	static void ClipTriangleAgainstPlane(CSG_Mesh& mesh, const _uint3& tris, const CSG_Plane& plane, vector<_uint3>& frontTris, vector<_uint3>& backTris);
	static void SplitPolygon(CSG_Mesh& polygon, const CSG_Plane& plane, const vector<_uint3>& triangles,
		vector<_uint3>& coplanar, vector<_uint3>& coplanarB, vector<_uint3>& FrontP, vector<_uint3>& BackP);
	static vector<_uint3> ClipPolygon(CSG_Mesh& mesh, const vector<_uint3>& triangles, unique_ptr<BSP_Node>& node);
	static void ClipTo(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB);

	static CSG_Mesh MakePolygon(const CSG_Mesh& mesh, const vector<_uint3>& tris);

	static void ReconstructMesh(CSG_Mesh& mesh, unique_ptr<BSP_Node>& node);
	static CSG_Mesh CSG_Union(CSG_Mesh& meshA, CSG_Mesh& meshB);
	static CSG_Mesh CSG_Subtract(CSG_Mesh& meshA, CSG_Mesh& meshB);
	static CSG_Mesh CSG_Intersect(CSG_Mesh& meshA, CSG_Mesh& meshB);
	static void CSG_Union(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB);
	static void CSG_Subtract(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB);
	static void CSG_Intersect(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB);


	static CSG_Mesh ConvertMeshData(const MESH_DATA& meshData, _float fMaterialIdx = 0.f);
	static shared_ptr<MESH_DATA> RevertMeshData(const CSG_Mesh& mesh, _bool ToAnim, _uint iBlendIdx = 0);

	static void CSG_ApplyMatrix(CSG_Mesh& mesh, _fmatrix matrix);



public:
	static _float3 LerpVec3(const _float3& v0, const _float3& v1, _float t)
	{
		_float3 rt;
		XMStoreFloat3(&rt, XMVectorLerp(XMLoadFloat3(&v0), XMLoadFloat3(&v1), t));
		return rt;
	}
	static DOUBLE3 LerpVec3(const DOUBLE3& v0, const DOUBLE3& v1, double t)
	{
		DOUBLE3 rt;
		//Lerp v0, v1,t
		rt = v0 + (v1 - v0) * t;
		return rt;
	}
	static _float2 LerpVec2(const _float2& v0, const _float2& v1, _float t)
	{
		_float2 rt;
		XMStoreFloat2(&rt, XMVectorLerp(XMLoadFloat2(&v0), XMLoadFloat2(&v1), t));
		return rt;
	}
	static _float3 Normalize(const _float3& v)
	{
		_float3 rt;
		XMStoreFloat3(&rt, XMVector3Normalize(XMLoadFloat3(&v)));
		return rt;
	}
	static CSG_Vertex LerpVertex(const CSG_Vertex& v0, const CSG_Vertex& v1, double t)
	{
		CSG_Vertex rt;
		rt.position = LerpVec3(v0.position, v1.position, t);
		rt.normal = Normalize(LerpVec3(v0.normal, v1.normal, t));
		//rt.tangent = Normalize(LerpVec3(v0.tangent, v1.tangent, t));
		rt.texcoord = LerpVec3(v0.texcoord, v1.texcoord, t);
		return rt;
	}
	static double GetPlaneIntersectionT(const DOUBLE3& a, const DOUBLE3& b, const CSG_Plane& plane) {
		auto& va = a;
		auto& vb = b;
		auto& n = plane.normal;

		double da = n.Dot(va) + plane.d;
		double db = n.Dot(vb) + plane.d;

		return da / (da - db); // 보간 비율 t
	}
	static _uint AddVertex(CSG_Mesh& mesh, const CSG_Vertex& vertex)
	{
		mesh.vertices.push_back(vertex.position);
		mesh.normals.push_back(vertex.normal);
		//mesh.tangents.push_back(vertex.tangent);
		mesh.texcoords.push_back(vertex.texcoord);

		return (_uint)mesh.vertices.size() - 1;
	}
};

END