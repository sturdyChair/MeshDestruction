# 코드 설명서

## CSG - Mesh Boolean

'''CSG_Manager.cpp

unique_ptr<BSP_Node> CCSG_Manager::BuildBSPTree(CSG_Mesh& mesh, const vector<_uint3>& triangles, const _float3& center)
{
	if (triangles.empty()) return nullptr;
	auto node = make_unique<BSP_Node>();
	const auto& BaseTriangle = triangles[rand() % triangles.size()];
	const auto& v0 = mesh.vertices[BaseTriangle.x], v1 = mesh.vertices[BaseTriangle.y], v2 = mesh.vertices[BaseTriangle.z];
	node->center = center;
	CSG_Plane plane{ v0,v1,v2 };
	node->splitter = plane;
	vector<_uint3> frontTris, backTris;
	node->coplanar.push_back(BaseTriangle);
	for (const auto& tri : triangles){
		if (tri.x == BaseTriangle.x && tri.y == BaseTriangle.y && tri.z == BaseTriangle.z){
			continue;
		}
		PlaneSide side = ClassifyTriangle(mesh, tri, plane);
		switch (side){
		case PlaneSide::Front:
			frontTris.push_back(tri);
			break;
		case PlaneSide::Back:
			backTris.push_back(tri);
			break;
		case PlaneSide::Coplanr:
			node->coplanar.push_back(tri);
			break;
		case PlaneSide::Spanning:{
			vector<_uint3> outFront, outBack;
			ClipTriangleAgainstPlane(mesh, tri, plane, outFront, outBack);
			frontTris.insert(frontTris.end(), outFront.begin(), outFront.end());
			backTris.insert(backTris.end(), outBack.begin(), outBack.end());
		}
		break;
		}
	}
	node->MakePolygon(mesh);
	if(frontTris.size()) node->front = BuildBSPTree(mesh, frontTris, center);
	if(backTris.size()) node->back = BuildBSPTree(mesh, backTris, center);
	return node;
}

'''
