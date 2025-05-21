#include "CSG_Manager.h"
#include "NvBlastExtAuthoringMesh.h"

BEGIN(Engine)
PlaneSide CCSG_Manager::ClassifyVertex(const DOUBLE3& vertex, const CSG_Plane& plane, double fEpsModi)
{
	double dist = plane.Distance(vertex);

	if (dist > CSG_EPSILON * fEpsModi) return PlaneSide::Front;
	else if (dist < -CSG_EPSILON * fEpsModi) return PlaneSide::Back;
	else return PlaneSide::Coplanr;
}
PlaneSide CCSG_Manager::ClassifyTriangle(const CSG_Mesh& mesh, const _uint3& tris, const CSG_Plane& plane)
{
	return ClassifyTriangleAgainstPlane(mesh.vertices[tris.x], mesh.vertices[tris.y], mesh.vertices[tris.z], plane);
}
PlaneSide CCSG_Manager::ClassifyTriangleAgainstPlane(const DOUBLE3& v0, const DOUBLE3& v1, const DOUBLE3& v2, const CSG_Plane& plane)
{
	double d0 = plane.Distance(v0);
	double d1 = plane.Distance(v1);
	double d2 = plane.Distance(v2);
	double fEpsilon = CSG_EPSILON;
	bool front = (d0 > fEpsilon) || (d1 > fEpsilon) || (d2 > fEpsilon);
	bool back = (d0 < -fEpsilon) || (d1 < -fEpsilon) || (d2 < -fEpsilon);

	if (front && back)
		return PlaneSide::Spanning;
	if (front)
		return PlaneSide::Front;
	if (back)
		return PlaneSide::Back;

    return PlaneSide::Coplanr;
}

void CCSG_Manager::ClipTriangleAgainstPlane(CSG_Mesh& mesh, const _uint3& tris, const CSG_Plane& plane, vector<_uint3>& frontTris, vector<_uint3>& backTris)
{
	array<_uint, 3> triangle = { tris.x, tris.y, tris.z };
	array<CSG_Vertex, 3> verts = { mesh.GetVertex(tris.x), mesh.GetVertex(tris.y), mesh.GetVertex(tris.z) };
	double fEpsModi = verts[0].position.x - verts[1].position.x;
	array<PlaneSide, 3> sides = { ClassifyVertex(verts[0].position, plane),
		ClassifyVertex(verts[1].position, plane), ClassifyVertex(verts[2].position, plane)};

	_uint front = 0;
	_uint back =  0;
	for (auto side : sides)
	{
		if (side == PlaneSide::Front)
			++front;
		else if (side == PlaneSide::Back)
			++back;
	}
	vector<_uint>  FrontIndices, BackIndices;
	FrontIndices.reserve(4);
	BackIndices.reserve(4);
	if (front && back)
	{
		for (_uint i = 0; i < 3; ++i)
		{
			if (sides[i] == PlaneSide::Front)
				FrontIndices.push_back(triangle[i]);
			else if (sides[i] == PlaneSide::Back)
				BackIndices.push_back(triangle[i]);
			else if (sides[i] == PlaneSide::Coplanr)
			{
				FrontIndices.push_back(triangle[i]);
				BackIndices.push_back(triangle[i]);
			}
			const CSG_Vertex& vA = verts[i];
			const CSG_Vertex& vB = verts[(i + 1) % 3];
			PlaneSide sideA = sides[i];
			PlaneSide sideB = sides[(i + 1) % 3];
			if ((sideA == PlaneSide::Front && sideB == PlaneSide::Back) ||
				(sideA == PlaneSide::Back && sideB == PlaneSide::Front))
			{
				double t = GetPlaneIntersectionT(vA.position, vB.position, plane);
				
				CSG_Vertex newVertex = LerpVertex(vA, vB, t);
				
				_uint iNewIdx = AddVertex(mesh, newVertex);
				
				FrontIndices.push_back(iNewIdx);
				BackIndices.push_back(iNewIdx);
			}
		}

		if (FrontIndices.size() == 3)
		{
			frontTris.push_back(_uint3(FrontIndices[0], FrontIndices[1], FrontIndices[2]));
		}
		else if (FrontIndices.size() == 4)
		{
			frontTris.push_back(_uint3(FrontIndices[0], FrontIndices[1], FrontIndices[2]));
			frontTris.push_back(_uint3(FrontIndices[0], FrontIndices[2], FrontIndices[3]));
		}
		else
		{
			assert(false);
		}

		if (BackIndices.size() == 3)
		{
			backTris.push_back(_uint3(BackIndices[0], BackIndices[1], BackIndices[2]));
		}
		else if (BackIndices.size() == 4)
		{
			backTris.push_back(_uint3(BackIndices[0], BackIndices[1], BackIndices[2]));
			backTris.push_back(_uint3(BackIndices[0], BackIndices[2], BackIndices[3]));
		}
		else
		{
			assert(false);
		}
	}
	else if (front)
	{
		frontTris.push_back(tris);
		return;
	}
	else if (back)
	{
		backTris.push_back(tris);
		return;
	}


}

void CCSG_Manager::SplitPolygon(CSG_Mesh& polygon, const CSG_Plane& plane, const vector<_uint3>& triangles,
	vector<_uint3>& coplanarF, vector<_uint3>& coplanarB, vector<_uint3>& FrontP, vector<_uint3>& BackP)
{
	for (const auto& tri : triangles)
	{
		PlaneSide side = ClassifyTriangle(polygon, tri, plane);
		switch (side)
		{
		case PlaneSide::Front:
			FrontP.push_back(tri);
			break;
		case PlaneSide::Back:
			BackP.push_back(tri);
			break;
		case PlaneSide::Coplanr:
		{
			CSG_Plane coplanarPlane(polygon.vertices[tri.x], polygon.vertices[tri.y], polygon.vertices[tri.z]);
			if (coplanarPlane.normal.Dot(plane.normal) > 0)
				coplanarF.push_back(tri);
			else
				coplanarB.push_back(tri);
			break;
		}
		case PlaneSide::Spanning:
		{
			vector<_uint3> outFront, outBack;
			ClipTriangleAgainstPlane(polygon, tri, plane, outFront, outBack);
			FrontP.insert(FrontP.end(), outFront.begin(), outFront.end());
			BackP.insert(BackP.end(), outBack.begin(), outBack.end());
			break;
		}
		}
	}
}

vector<_uint3> CCSG_Manager::ClipPolygon(CSG_Mesh& mesh, const vector<_uint3>& triangles, unique_ptr<BSP_Node>& node)
{
	assert(node);
	vector<_uint3> rt;
	vector<_uint3> front, back;
	SplitPolygon(mesh, node->splitter, triangles,
		front, back, front, back);
	if (node->front)
	{
		vector<_uint3> tmp = ClipPolygon(mesh, front, node->front);
		rt.insert(rt.end(), tmp.begin(), tmp.end());
	}
	else
	{
		rt.insert(rt.end(), front.begin(), front.end());
	}
	if(node->back)
	{
		vector<_uint3> tmp = ClipPolygon(mesh, back, node->back);
		rt.insert(rt.end(), tmp.begin(), tmp.end());
	}
	return rt;
}

void CCSG_Manager::ClipTo(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB)
{
	vector<_uint3> triangles;
	for (size_t i = 0; i < nodeA->polygon.indices.size(); i += 3)
	{
		triangles.push_back(_uint3(nodeA->polygon.indices[i], nodeA->polygon.indices[i + 1], nodeA->polygon.indices[i + 2]));
	}
	vector<_uint3> cliped = ClipPolygon(nodeA->polygon, triangles, nodeB);
	nodeA->polygon = MakePolygon(nodeA->polygon, cliped);
	if (nodeA->front)
	{
		ClipTo(nodeA->front, nodeB);
	}
	if (nodeA->back)
	{
		ClipTo(nodeA->back, nodeB);
	}
}

CSG_Mesh CCSG_Manager::MakePolygon(const CSG_Mesh& mesh, const vector<_uint3>& tris)
{
	CSG_Mesh polygon;
	polygon.vertices.reserve(tris.size() * 3);
	polygon.normals.reserve(tris.size() * 3);
	//polygon.tangents.reserve(tris.size() * 3);
	polygon.texcoords.reserve(tris.size() * 3);
	polygon.indices.reserve(tris.size() * 3);
	map<_uint, _uint> indexMap;
	for (const auto& tri : tris)
	{
		array<_uint, 3> triangle = { tri.x, tri.y, tri.z };

		for (auto i : triangle)
		{
			auto it = indexMap.find(i);
			if (it == indexMap.end())
			{
				CSG_Vertex v = mesh.GetVertex(i);
				_uint newIndex = polygon.vertices.size();
				polygon.vertices.push_back(mesh.vertices[i]);
				polygon.normals.push_back(mesh.normals[i]);
				//polygon.tangents.push_back(mesh.tangents[i]);
				polygon.texcoords.push_back(mesh.texcoords[i]);
				polygon.indices.push_back(newIndex);
				indexMap[i] = newIndex;
			}
			else
			{
				polygon.indices.push_back(it->second);
			}
		}
	}
	polygon.iOriginalSize = (_uint)polygon.vertices.size();
	polygon.iOriginalIdxSize = (_uint)polygon.indices.size();
	return polygon;
}

void CCSG_Manager::ReconstructMesh(CSG_Mesh& mesh, unique_ptr<BSP_Node>& node)
{
	if (node->front)
		ReconstructMesh(mesh, node->front);
	if (node->back)
		ReconstructMesh(mesh, node->back);

	_uint iOriginalSize = (_uint)mesh.vertices.size();
	mesh.vertices.insert(mesh.vertices.end(), node->polygon.vertices.begin(), node->polygon.vertices.end());
	mesh.normals.insert(mesh.normals.end(), node->polygon.normals.begin(), node->polygon.normals.end());
	//mesh.tangents.insert(mesh.tangents.end(), node->polygon.tangents.begin(), node->polygon.tangents.end());
	mesh.texcoords.insert(mesh.texcoords.end(), node->polygon.texcoords.begin(), node->polygon.texcoords.end());
	for_each(node->polygon.indices.begin(), node->polygon.indices.end(), [&](_uint index) {
		mesh.indices.push_back(iOriginalSize + index);
		});

}

CSG_Mesh CCSG_Manager::CSG_Union(CSG_Mesh& meshA, CSG_Mesh& meshB)
{
	// Build BSP trees for both meshes
	vector<_uint3> trianglesA, trianglesB;
	for (size_t i = 0; i < meshA.indices.size(); i += 3)
	{
		trianglesA.push_back(_uint3(meshA.indices[i], meshA.indices[i + 1], meshA.indices[i + 2]));
	}
	for (size_t i = 0; i < meshB.indices.size(); i += 3)
	{
		trianglesB.push_back(_uint3(meshB.indices[i], meshB.indices[i + 1], meshB.indices[i + 2]));
	}
	auto nodeA = BuildBSPTree(meshA, trianglesA, meshA.center);
	auto nodeB = BuildBSPTree(meshB, trianglesB, meshB.center);

	CSG_Union(nodeA, nodeB);
	// Merge the two meshes
	CSG_Mesh resultMesh;
	ReconstructMesh(resultMesh, nodeA);
	ReconstructMesh(resultMesh, nodeB);
	resultMesh.Optimize();
	return resultMesh;
}

CSG_Mesh CCSG_Manager::CSG_Subtract(CSG_Mesh& meshA, CSG_Mesh& meshB)
{
	// Build BSP trees for both meshes
	vector<_uint3> trianglesA, trianglesB;
	for (size_t i = 0; i < meshA.indices.size(); i += 3)
	{
		trianglesA.push_back(_uint3(meshA.indices[i], meshA.indices[i + 1], meshA.indices[i + 2]));
	}
	for (size_t i = 0; i < meshB.indices.size(); i += 3)
	{
		trianglesB.push_back(_uint3(meshB.indices[i], meshB.indices[i + 1], meshB.indices[i + 2]));
	}
	auto nodeA = BuildBSPTree(meshA, trianglesA, meshA.center);
	auto nodeB = BuildBSPTree(meshB, trianglesB, meshB.center);
	CSG_Subtract(nodeA, nodeB);
	CSG_Mesh resultMesh;
	ReconstructMesh(resultMesh, nodeA);
	resultMesh.Optimize();
	return resultMesh;
}

CSG_Mesh CCSG_Manager::CSG_Intersect(CSG_Mesh& meshA, CSG_Mesh& meshB)
{
	// Build BSP trees for both meshes
	vector<_uint3> trianglesA, trianglesB;
	for (size_t i = 0; i < meshA.indices.size(); i += 3)
	{
		trianglesA.push_back(_uint3(meshA.indices[i], meshA.indices[i + 1], meshA.indices[i + 2]));
	}
	for (size_t i = 0; i < meshB.indices.size(); i += 3)
	{
		trianglesB.push_back(_uint3(meshB.indices[i], meshB.indices[i + 1], meshB.indices[i + 2]));
	}
	auto nodeA = BuildBSPTree(meshA, trianglesA, meshA.center);
	auto nodeB = BuildBSPTree(meshB, trianglesB, meshB.center);
	CSG_Intersect(nodeA, nodeB);
	CSG_Mesh resultMesh;
	ReconstructMesh(resultMesh, nodeA);
	resultMesh.Optimize();
	return resultMesh;
}

void CCSG_Manager::CSG_Union(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB)
{
	//Clip the meshes against each other
	ClipTo(nodeA, nodeB);
	ClipTo(nodeB, nodeA);
	// Invert the second mesh
	InvertBSPTree(nodeB);
	// Clip the inverted mesh against the first one
	ClipTo(nodeB, nodeA);
	// Revert the second mesh
	InvertBSPTree(nodeB);

	return;
}

void CCSG_Manager::CSG_Subtract(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB)
{
	InvertBSPTree(nodeA);
	// Clip the meshes against each other
	ClipTo(nodeA, nodeB);
	ClipTo(nodeB, nodeA);
	// Invert the second mesh
	InvertBSPTree(nodeB);
	// Clip the inverted mesh against the first one
	ClipTo(nodeB, nodeA);
	// Revert the second mesh
	InvertBSPTree(nodeB);

	// Merge the two meshes
	CSG_Mesh bMesh;
	ReconstructMesh(bMesh, nodeB);
	AppendBSPTree(nodeA, bMesh);

	InvertBSPTree(nodeA);
	return;
}

void CCSG_Manager::CSG_Intersect(unique_ptr<BSP_Node>& nodeA, unique_ptr<BSP_Node>& nodeB)
{
	InvertBSPTree(nodeA);
	// Clip second mesh against the first one
	ClipTo(nodeB, nodeA);
	// Invert the second mesh
	InvertBSPTree(nodeB);
	// Clip the meshes against each other
	ClipTo(nodeA, nodeB);
	ClipTo(nodeB, nodeA);
	// Merge the two meshes
	CSG_Mesh bMesh;
	ReconstructMesh(bMesh, nodeB);
	AppendBSPTree(nodeA, bMesh);

	InvertBSPTree(nodeA);
	return;
}

CSG_Mesh CCSG_Manager::ConvertMeshData(const MESH_DATA& meshData, _float fMaterialIdx)
{
	CSG_Mesh mesh;
	mesh.vertices.resize(meshData.iNumVertices);
	mesh.normals.resize(meshData.iNumVertices);
	//mesh.tangents.resize(meshData.iNumVertices);
	mesh.texcoords.resize(meshData.iNumVertices);
	mesh.indices.resize(meshData.iNumFaces * 3);
	double fMaxX = -FLT_MAX;
	double fMinX = FLT_MAX;
	double fMaxY = -FLT_MAX;
	double fMinY = FLT_MAX;
	double fMaxZ = -FLT_MAX;
	double fMinZ = FLT_MAX;

	_bool bIsAnim = meshData.eModelType == MODEL_TYPE::ANIM;
	if (!bIsAnim) 
	{
		for (size_t i = 0; i < meshData.iNumVertices; ++i)
		{
			mesh.vertices[i].x = meshData.pVertices[i].vPosition.x;
			mesh.vertices[i].y = meshData.pVertices[i].vPosition.y;
			mesh.vertices[i].z = meshData.pVertices[i].vPosition.z;
			XMStoreFloat3(&mesh.normals[i],XMVector3Normalize(XMLoadFloat3(&meshData.pVertices[i].vNormal)));
			//mesh.tangents[i] = meshData.pVertices[i].vTangent;
			mesh.texcoords[i].x = meshData.pVertices[i].vTexcoord.x;
			mesh.texcoords[i].y = meshData.pVertices[i].vTexcoord.y;
			mesh.texcoords[i].z = fMaterialIdx;
			fMaxX = max(fMaxX, mesh.vertices[i].x);
			fMinX = min(fMinX, mesh.vertices[i].x);
			fMaxY = max(fMaxY, mesh.vertices[i].y);
			fMinY = min(fMinY, mesh.vertices[i].y);
			fMaxZ = max(fMaxZ, mesh.vertices[i].z);
			fMinZ = min(fMinZ, mesh.vertices[i].z);
		}
	}
	else
	{
		for (size_t i = 0; i < meshData.iNumVertices; ++i)
		{
			mesh.vertices[i].x = meshData.pAnimVertices[i].vPosition.x;
			mesh.vertices[i].y = meshData.pAnimVertices[i].vPosition.y;
			mesh.vertices[i].z = meshData.pAnimVertices[i].vPosition.z;
			mesh.normals[i] = meshData.pAnimVertices[i].vNormal;
			//mesh.tangents[i] = meshData.pAnimVertices[i].vTangent;
			mesh.texcoords[i].x = meshData.pAnimVertices[i].vTexcoord.x;
			mesh.texcoords[i].y = meshData.pAnimVertices[i].vTexcoord.y;
			mesh.texcoords[i].z = fMaterialIdx;
			fMaxX = max(fMaxX, mesh.vertices[i].x);
			fMinX = min(fMinX, mesh.vertices[i].x);
			fMaxY = max(fMaxY, mesh.vertices[i].y);
			fMinY = min(fMinY, mesh.vertices[i].y);
			fMaxZ = max(fMaxZ, mesh.vertices[i].z);
			fMinZ = min(fMinZ, mesh.vertices[i].z);
		}
	}
	for (size_t i = 0; i < meshData.iNumFaces; ++i)
	{
		mesh.indices[i * 3] = meshData.pIndices[i]._1;
		mesh.indices[i * 3 + 1] = meshData.pIndices[i]._2;
		mesh.indices[i * 3 + 2] = meshData.pIndices[i]._3;
	}
	mesh.iOriginalIdxSize = mesh.indices.size();
	mesh.iOriginalSize = mesh.vertices.size();
	mesh.center = _float3( (fMaxX + fMinX) / 2.f, (fMaxY + fMinY) / 2.f, (fMaxZ + fMinZ) / 2.f);
	return mesh;
}


shared_ptr<MESH_DATA> CCSG_Manager::RevertMeshData(const CSG_Mesh& mesh, _bool ToAnim, _uint iBlendIdx)
{
	shared_ptr<MESH_DATA> meshData = make_shared<MESH_DATA>();
	if (mesh.vertices.empty() || mesh.indices.empty())
		return meshData;

	meshData->iNumVertices = mesh.vertices.size();
	meshData->iNumFaces = mesh.indices.size() / 3;
	if(ToAnim)
		meshData->pAnimVertices = shared_ptr<VTXANIM[]>(DBG_NEW VTXANIM[meshData->iNumVertices]);
	else
		meshData->pVertices = shared_ptr<VTXMESH[]>(DBG_NEW VTXMESH[meshData->iNumVertices]);
	meshData->pIndices = shared_ptr<FACEINDICES32[]>(DBG_NEW FACEINDICES32[meshData->iNumFaces]);
	meshData->iMaterialIndex = 0;
	meshData->eModelType = ToAnim ? MODEL_TYPE::ANIM : MODEL_TYPE::NONANIM;
	meshData->iNumBones = 0;
	meshData->Bone_Datas.clear();
	meshData->szName = "";
	for (size_t i = 0; i < meshData->iNumVertices; ++i)
	{
		if (ToAnim)
		{
			meshData->pAnimVertices[i].vPosition.x = mesh.vertices[i].x;
			meshData->pAnimVertices[i].vPosition.y = mesh.vertices[i].y;
			meshData->pAnimVertices[i].vPosition.z = mesh.vertices[i].z;
			meshData->pAnimVertices[i].vNormal = mesh.normals[i];
			meshData->pAnimVertices[i].vTangent = { 0.f,0.f,1.f };
			meshData->pAnimVertices[i].vTexcoord.x = mesh.texcoords[i].x;
			meshData->pAnimVertices[i].vTexcoord.y = mesh.texcoords[i].y;
			meshData->pAnimVertices[i].vBlendIndex.x = iBlendIdx;

			meshData->pAnimVertices[i].vBlendWeight.x = 1.f;
			meshData->pAnimVertices[i].vBlendWeight.y = 0.f;
			meshData->pAnimVertices[i].vBlendWeight.z = 0.f;
			meshData->pAnimVertices[i].vBlendWeight.w = mesh.texcoords[i].z;
		}
		else
		{
			meshData->pVertices[i].vPosition.x = mesh.vertices[i].x;
			meshData->pVertices[i].vPosition.y = mesh.vertices[i].y;
			meshData->pVertices[i].vPosition.z = mesh.vertices[i].z;
			meshData->pVertices[i].vNormal = mesh.normals[i];
			meshData->pVertices[i].vTangent = { 0.f,0.f,1.f };
			meshData->pVertices[i].vTexcoord.x = mesh.texcoords[i].x + mesh.texcoords[i].z;
			meshData->pVertices[i].vTexcoord.y = mesh.texcoords[i].y + mesh.texcoords[i].z;
		}
	}
	for (size_t i = 0; i < meshData->iNumFaces; ++i)
	{
		meshData->pIndices[i]._1 = mesh.indices[i * 3];
		meshData->pIndices[i]._2 = mesh.indices[i * 3 + 1];
		meshData->pIndices[i]._3 = mesh.indices[i * 3 + 2];
		meshData->pIndices[i].numIndices = 3;
	}

	return meshData;
}

void CCSG_Manager::CSG_ApplyMatrix(CSG_Mesh& mesh, _fmatrix matrix)
{
	_uint size = mesh.vertices.size();
	for (_uint i = 0; i < size; ++i)
	{
		_vector pos = { mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z, 1.f };
		_vector norm = XMLoadFloat3(&mesh.normals[i]);
		//_vector tang = XMLoadFloat3(&mesh.tangents[i]);
		pos = XMVector4Transform(pos, matrix);
		norm = XMVector3Normalize(XMVector3TransformNormal(norm, matrix));
		//tang = XMVector3Transform(tang, matrix);
		mesh.vertices[i] = { pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2] };
		XMStoreFloat3(&mesh.normals[i], norm);
		//XMStoreFloat3(&mesh.tangents[i], tang);
	}
}







unique_ptr<BSP_Node> CCSG_Manager::BuildBSPTree(CSG_Mesh& mesh, const vector<_uint3>& triangles, const _float3& center)
{
	if (triangles.empty())
		return nullptr;
	auto node = make_unique<BSP_Node>();
	const auto& BaseTriangle = triangles[rand() % triangles.size()];
	const auto& v0 = mesh.vertices[BaseTriangle.x];
	const auto& v1 = mesh.vertices[BaseTriangle.y];
	const auto& v2 = mesh.vertices[BaseTriangle.z];
	node->center = center;
	CSG_Plane plane{ v0,v1,v2 };
	//node->splitter = CSG_Plane(plane.normal, center);
	//plane = node->splitter;
	node->splitter = plane;
	vector<_uint3> frontTris, backTris;
	node->coplanar.push_back(BaseTriangle);

	for (const auto& tri : triangles)
	{
		if (tri.x == BaseTriangle.x && tri.y == BaseTriangle.y && tri.z == BaseTriangle.z)
		{
			continue;
		}

		PlaneSide side = ClassifyTriangle(mesh, tri, plane);

		switch (side)
		{
		case PlaneSide::Front:
			frontTris.push_back(tri);
			break;
		case PlaneSide::Back:
			backTris.push_back(tri);
			break;
		case PlaneSide::Coplanr:
			node->coplanar.push_back(tri);
			break;
		case PlaneSide::Spanning:
		{
			vector<_uint3> outFront, outBack;

			ClipTriangleAgainstPlane(mesh, tri, plane, outFront, outBack);
			frontTris.insert(frontTris.end(), outFront.begin(), outFront.end());
			backTris.insert(backTris.end(), outBack.begin(), outBack.end());

		}
		break;
		}
	}
	node->MakePolygon(mesh);
	if(frontTris.size())
		node->front = BuildBSPTree(mesh, frontTris, center);
	if(backTris.size())
		node->back = BuildBSPTree(mesh, backTris, center);

	return node;
}

void CCSG_Manager::AppendBSPTree(unique_ptr<BSP_Node>& node, CSG_Mesh& mesh)
{
	vector<_uint3> triangles;
	for (size_t i = 0; i < mesh.indices.size(); i += 3)
	{
		triangles.push_back(_uint3(mesh.indices[i], mesh.indices[i + 1], mesh.indices[i + 2]));
	}
	vector<_uint3> frontTris, backTris, coplanar;
	for (const auto& tri : triangles)
	{
		PlaneSide side = ClassifyTriangle(mesh, tri, node->splitter);
		switch (side)
		{
		case PlaneSide::Front:
			frontTris.push_back(tri);
			break;
		case PlaneSide::Back:
			backTris.push_back(tri);
			break;
		case PlaneSide::Coplanr:
			coplanar.push_back(tri);
			break;
		case PlaneSide::Spanning:
			vector<_uint3> outFront, outBack;
			ClipTriangleAgainstPlane(mesh, tri, node->splitter, outFront, outBack);
			frontTris.insert(frontTris.end(), outFront.begin(), outFront.end());
			backTris.insert(backTris.end(), outBack.begin(), outBack.end());
			break;
		}
	}

	_uint iOriginalSize = (_uint)node->polygon.vertices.size();
	map<_uint, _uint> indexMap;
	for (auto& tri : coplanar)
	{
		array<_uint, 3> t = { tri.x, tri.y, tri.z };
		for (auto i : t)
		{
			auto it = indexMap.find(i);
			if (it == indexMap.end())
			{
				CSG_Vertex v = mesh.GetVertex(i);
				_uint newIndex = node->polygon.vertices.size();
				node->polygon.vertices.push_back(mesh.vertices[i]);
				node->polygon.normals.push_back(mesh.normals[i]);
				//node->polygon.tangents.push_back(mesh.tangents[i]);
				node->polygon.texcoords.push_back(mesh.texcoords[i]);
				node->polygon.indices.push_back(newIndex);
				indexMap[i] = newIndex;
			}
			else
			{
				node->polygon.indices.push_back(it->second);
			}
		}
	}

	node->polygon.iOriginalSize = (_uint)node->polygon.vertices.size();	
	node->polygon.iOriginalIdxSize = (_uint)node->polygon.indices.size();

	if (frontTris.size() > 0)
	{
		if (node->front)
		{
			CSG_Mesh frontMesh = MakePolygon(mesh, frontTris);
			AppendBSPTree(node->front, frontMesh);
		}
		else
			node->front = BuildBSPTree(mesh, frontTris, node->center);
	}
	if (backTris.size() > 0)
	{
		if (node->back)
		{
			CSG_Mesh backMesh = MakePolygon(mesh, backTris);
			AppendBSPTree(node->back, backMesh);
		}
		else
			node->back = BuildBSPTree(mesh, backTris, node->center);
	}
}

unique_ptr<BSP_Node>& CCSG_Manager::InvertBSPTree(unique_ptr<BSP_Node>& node)
{
	if (node->front)
		InvertBSPTree(node->front);
	if (node->back)
		InvertBSPTree(node->back);
	for_each(node->polygon.normals.begin(), node->polygon.normals.end(), [](_float3& v) { XMStoreFloat3(&v, -XMLoadFloat3(&v)); });
	//for_each(node->polygon.tangents.begin(), node->polygon.tangents.end(), [](_float3& v) { XMStoreFloat3(&v, -XMLoadFloat3(&v)); });
	for (auto& tris : node->coplanar)
	{
		swap(tris.y, tris.z);
	}

	for (_uint i = 0; i < node->polygon.indices.size(); i += 3)
	{
		swap(node->polygon.indices[i + 1], node->polygon.indices[i + 2]);
	}
	node->splitter.normal.x = -node->splitter.normal.x;
	node->splitter.normal.y = -node->splitter.normal.y;
	node->splitter.normal.z = -node->splitter.normal.z;
	node->splitter.d = -node->splitter.d;

	node->Inverted = !node->Inverted;
	node->front.swap(node->back);
	
	return node;
}

unique_ptr<BSP_Node>& CCSG_Manager::RevertBSPTree(unique_ptr<BSP_Node>& node)
{
	if (node->front)
		RevertBSPTree(node->front);
	if (node->back)
		RevertBSPTree(node->back);
	node->Inverted = false;
	return node;
}




END