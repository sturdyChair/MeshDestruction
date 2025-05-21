#pragma once
#include "Engine_Defines.h"
#include <iostream>

namespace std
{
	template<>
	struct hash<DOUBLE3> {
		size_t operator()(const DOUBLE3& v) const {
			auto hash_combine = [](size_t h, double d) {
				size_t hashed = std::hash<uint64_t>()(*reinterpret_cast<const uint64_t*>(&d));
				return h ^ (hashed + 0x9e3779b9 + (h << 6) + (h >> 2));
				};
			size_t h = 0;
			h = hash_combine(h, v.x);
			h = hash_combine(h, v.y);
			h = hash_combine(h, v.z);
			return h;
		}
	};

}

BEGIN(Engine)
constexpr double CSG_EPSILON = 1e-10;


struct CSG_Vertex
{
	static constexpr _float snapSize = 1e-5f;
	DOUBLE3 position;
	_float3 normal;
	//_float3 tangent;
	_float3 texcoord;
	CSG_Vertex() :position{ 0.f,0.f,0.f }, normal{ 0.f,0.f,1.f },/* tangent{ 1.f,0.f,0.f },*/ texcoord{ 0.f,0.f, 0.f } {}
	CSG_Vertex(const DOUBLE3& pos) :position{ pos }, normal{ 0.f,0.f,1.f }, /*tangent{ 1.f,0.f,0.f },*/ texcoord{ 0.f,0.f, 0.f } { }
	CSG_Vertex(const DOUBLE3& pos, const _float3& norm, /*const _float3& tan,*/ const _float3 tex) : position{ pos }, normal{ norm }, /*tangent{ tan },*/ texcoord{ tex } { }

	void Snap()
	{
		position.x = floor(position.x / snapSize) * snapSize;
		position.y = floor(position.y / snapSize) * snapSize;
		position.z = floor(position.z / snapSize) * snapSize;
	}
	bool operator==(const CSG_Vertex& other) const {
	
		if (!(position == other.position) || XMVectorGetX(XMVectorNotEqual(XMLoadFloat3(&normal),XMLoadFloat3(&other.normal))))
			return false;
		if (XMVectorGetX(XMVectorNotEqual(XMLoadFloat3(&texcoord),XMLoadFloat3(&other.texcoord))))
			return false;
		return true;
	}
};
END
namespace std {
	template <>
	struct hash<CSG_Vertex> {
		std::size_t operator()(const CSG_Vertex& v) const {
			size_t h = 0;
			
			h ^= std::hash<float>()(v.position.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.normal.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
			
			h ^= std::hash<float>()(v.position.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.normal.y) + 0x9e3779b9 + (h << 6) + (h >> 2);

			h ^= std::hash<float>()(v.position.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.normal.z) + 0x9e3779b9 + (h << 6) + (h >> 2);

		
			h ^= std::hash<float>()(v.texcoord.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.texcoord.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<float>()(v.texcoord.z) + 0x9e3779b9 + (h << 6) + (h >> 2);


			return h;
		}
	};
}
BEGIN(Engine)
class CSG_Mesh
{
public:
	CSG_Mesh() = default;
	CSG_Mesh(const CSG_Mesh& rhs) = default;
	CSG_Mesh(CSG_Mesh&& rhs) noexcept = default;
	~CSG_Mesh() = default;

	CSG_Mesh& operator=(const CSG_Mesh& rhs) = default;
	CSG_Mesh& operator=(CSG_Mesh&& rhs) noexcept = default;

	vector<DOUBLE3> vertices;
	vector<_float3> normals;
	//vector<_float3> tangents;
	vector<_float3> texcoords;
	vector<_uint> indices;
	_float3 center;
	_uint iOriginalSize;
	_uint iOriginalIdxSize;

	_uint AddVertex(CSG_Vertex& vertex)
	{
		vertices.push_back(vertex.position);
		normals.push_back(vertex.normal);
		//tangents.push_back(vertex.tangent);
		texcoords.push_back(vertex.texcoord);

		indices.push_back((_uint)vertices.size() - 1);

		return (_uint)vertices.size() - 1;
	}

	void AddTriangle(_uint i0, _uint i1, _uint i2)
	{
		indices.push_back(i0);
		indices.push_back(i1);
		indices.push_back(i2);
	}
	void Clear()
	{
		vertices.clear();
		normals.clear();
		texcoords.clear();
		indices.clear();
		iOriginalSize = 0;
	}
	void Originalize()
	{
		vertices.resize(iOriginalSize);
		normals.resize(iOriginalSize);
		//tangents.resize(iOriginalSize);
		texcoords.resize(iOriginalSize);
		indices.resize(iOriginalIdxSize);
	}

	CSG_Vertex GetVertex(_uint index) const
	{
		return CSG_Vertex(vertices[index], normals[index],/* tangents[index],*/ texcoords[index]);
	}

	void Snap()
	{
		for (auto& vertex : vertices)
		{
			vertex.x = floor(vertex.x / CSG_Vertex::snapSize) * CSG_Vertex::snapSize;
			vertex.y = floor(vertex.y / CSG_Vertex::snapSize) * CSG_Vertex::snapSize;
			vertex.z = floor(vertex.z / CSG_Vertex::snapSize) * CSG_Vertex::snapSize;
		}
	}
	void Optimize()
	{
		//cout << "Before Optimize | vertex: " << vertices.size() << "| idx: " << indices.size() << '\n';
		Snap();
		unordered_multimap<CSG_Vertex, _uint, std::hash<CSG_Vertex>> indexMap;
		vector<DOUBLE3> newVertices;
		vector<_vector> newNormals;
		//vector<_vector> newTangents;
		vector<_float3> newTexcoords;
		vector<_uint> newIndices;

		newVertices.reserve(vertices.size());
		newNormals.reserve(normals.size());
		//newTangents.reserve(tangents.size());
		newTexcoords.reserve(texcoords.size());
		newIndices.reserve(indices.size());

		for (size_t i = 0; i < indices.size(); ++i)
		{
			_uint idx = indices[i];
			auto it = indexMap.find(GetVertex(idx));
			if (it == indexMap.end())
			{
				indexMap.insert({ GetVertex(idx), (_uint)newVertices.size() });
				newVertices.push_back(vertices[idx]);
				newNormals.push_back(XMVector3Normalize(XMLoadFloat3(&normals[idx])));
				//newTangents.push_back(XMVector3Normalize(XMLoadFloat3(&tangents[idx])));
				newTexcoords.push_back(texcoords[idx]);
				newIndices.push_back((_uint)newVertices.size() - 1);
			}
			else
			{
				auto range = indexMap.equal_range(it->first);
				_bool bFound = false;
				for (auto it2 = range.first; it2 != range.second; ++it2)
				{
					if (it2->second == it->second)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					indexMap.insert({ GetVertex(idx), (_uint)newVertices.size() });
					newVertices.push_back(vertices[idx]);
					newNormals.push_back(XMVector3Normalize(XMLoadFloat3(&normals[idx])));
					//newTangents.push_back(XMVector3Normalize(XMLoadFloat3(&tangents[idx])));
					newTexcoords.push_back(texcoords[idx]);
					newIndices.push_back((_uint)newVertices.size() - 1);
				}
				else
				{
					newIndices.push_back(it->second);
					newNormals[it->second] = XMVectorAdd(newNormals[it->second], XMVector3Normalize(XMLoadFloat3(&normals[idx])));
					//newTangents[it->second] = XMVectorAdd(newTangents[it->second], XMVector3Normalize(XMLoadFloat3(&tangents[idx])));
				}
			}
		}
		normals.resize(newNormals.size());
		//tangents.resize(newTangents.size());
		texcoords.resize(newTexcoords.size());
		for (size_t i = 0; i < newNormals.size(); ++i)
		{
			XMStoreFloat3(&normals[i], XMVector3Normalize(newNormals[i]));
			//XMStoreFloat3(&tangents[i], XMVector3Normalize(newTangents[i]));
		}
		vertices.swap(newVertices);
		texcoords.swap(newTexcoords);

		indices.swap(newIndices);
		iOriginalSize = (_uint)vertices.size();
		iOriginalIdxSize = (_uint)indices.size();
		//cout << "After Optimize | vertex: " << iOriginalSize << "| idx: " << iOriginalIdxSize << '\n';
	}
};


enum class PlaneSide
{
	Front,
	Back,
	Coplanr,
	Spanning
};

struct CSG_Plane
{
	DOUBLE3 normal;
	double d;

	double Distance(const DOUBLE3& point) const
	{
		return normal.Dot(point) + d;
	}

	CSG_Plane() :normal{ 0.,0.,1. }, d{ 0. } {}

	CSG_Plane(const DOUBLE3& a, const DOUBLE3& b, const DOUBLE3& c) {

		DOUBLE3 ab = b - a;
		DOUBLE3 ac = c - a;
		normal = ab.Cross(ac);
		normal.Normalize();
		
		d = -(normal.Dot(a));
	}
	CSG_Plane(const DOUBLE3& normal, const DOUBLE3 point)
	{
		this->normal = normal;
		d = -(normal.Dot(point));
	}
};

struct BSP_Node
{
	CSG_Plane splitter;
	vector<_uint3> coplanar;
	unique_ptr<BSP_Node> front;
	unique_ptr<BSP_Node> back;
	
	_uint iNumFront = 0;
	_uint iNumBack = 0;

	CSG_Mesh polygon;
	_float3 center;

	_bool Inverted = false;


	void MakePolygon(const CSG_Mesh& mesh);
	unique_ptr<BSP_Node> Clone() const
	{
		auto node = make_unique<BSP_Node>();
		node->splitter = splitter;
		node->coplanar = coplanar;
		node->front = front ? front->Clone() : nullptr;
		node->back = back ? back->Clone() : nullptr;
		node->polygon = polygon;
		node->center = center;
		node->Inverted = Inverted;
		return node;
	}

};




END