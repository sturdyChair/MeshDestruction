#pragma once

namespace Engine
{
    template < typename T, typename Func, std::size_t... I >
    void tuple_for_each_impl(const T& t, Func f, std::index_sequence<I...>) {
        (f(std::get<I>(t)), ...);
    }

    template < typename ...Ts, typename Func >
    void tuple_for_each(std::tuple<Ts...>& t, Func f) {
        tuple_for_each_impl(t, f, std::index_sequence_for<Ts...>{});
    }

    _float Float3Dist(const _float3& v1, const _float3& v2)
    {
        return  XMVector3Length( XMLoadFloat3(&v1) - XMLoadFloat3(&v2)).m128_f32[0];
    }

	template<typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)
			iRefCnt = pInstance->AddRef();

		return iRefCnt;
	}


	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();
			pInstance = nullptr;
		}

		return iRefCnt;
	}


	template<typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete[] pPointer;
			pPointer = nullptr;
		}
	}

	Engine::_float4x4 Matrix_Inverse(const Engine::_float4x4& Matrix);

	_float3 Get_Cursor_Pos(HWND hWnd);

	void XMFloat4x4ToFloatArray(const XMFLOAT4X4& mat, float out[16]);

	void FloatArrayToXMFloat4x4(const float in[16], XMFLOAT4X4& mat);
	
	template<typename T>
	std::ostream& write_typed_data(std::ostream& stream, const T& value) {
		return stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	template<typename T>
	std::istream& read_typed_data(std::istream& stream, T& value) {
		return stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	}
	PxMat44 ENGINE_DLL XMMatrixToPxMat(_fmatrix matrix);
	_matrix ENGINE_DLL PxMatToXMMatrix(PxMat44 matrix);
	_matrix ENGINE_DLL XMMatrixNormalize(_fmatrix matrix);

	_float fRand();
	_float fRand(_float min, _float max);

    void computeLocalBasis(const _float3&normal, _float3& u, _float3& v) {
        _float3 up = fabs(normal.z) < 0.999 ? _float3{ 0, 0, 1 } : _float3{ 1, 0, 0 };
        // u = normal x up
        u = { normal.y * up.z - normal.z * up.y,
             normal.z * up.x - normal.x * up.z,
             normal.x * up.y - normal.y * up.x };

        // normalize u
        _float len = std::sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
        u = { u.x / len, u.y / len, u.z / len };

        // v = normal x u
        v = { normal.y * u.z - normal.z * u.y,
             normal.z * u.x - normal.x * u.z,
             normal.x * u.y - normal.y * u.x };
    }

    _float2 projectTo2D(const _float3& point, const _float3& origin, const _float3& u, const _float3& v) 
    {
        _float3 p = { point.x - origin.x, point.y - origin.y, point.z - origin.z };
        return { p.x * u.x + p.y * u.y + p.z * u.z, p.x * v.x + p.y * v.y + p.z * v.z };
    }

    // Cross product to check convexity in 2D
    bool isConvex(const _float2& a, const _float2& b, const _float2& c) 
    {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) < 1e-4f; // 시계방향
    }

    bool isPointInTriangle(const _float2& p, const _float2& a, const _float2& b, const _float2& c) 
    {
        auto sign = [](const _float2& p1, const _float2& p2, const _float2& p3) {
            return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
            };
        bool b1 = sign(p, a, b) < 1e-4f;
        bool b2 = sign(p, b, c) < 1e-4f;
        bool b3 = sign(p, c, a) < 1e-4f;
        return (b1 == b2 && b2 == b3);
    }

    // Triangulation 함수
    vector<tuple<int, int, int>> triangulate3DPolygon(const vector<_float3>& vertices, const _float3& normal) 
    {
        _float3 u, v;
        computeLocalBasis(normal, u, v);
        _float3 origin = vertices[0];

        // 2D 변환
        std::vector<_float2> projected;
        for (auto& pt : vertices) {
            projected.push_back(projectTo2D(pt, origin, u, v));
        }

        // Ear Clipping
        vector<int> indices(vertices.size());
        for (int i = 0; i < vertices.size(); ++i) indices[i] = i;

        vector<tuple<int, int, int>> triangles;
        map<pair<int, int>, vector<_uint>> edgeTri;

        while (indices.size() > 3) {
            bool earFound = false;
            for (int i = 0; i < indices.size(); ++i) {
                int prev = indices[(i - 1 + indices.size()) % indices.size()];
                int curr = indices[i];
                int next = indices[(i + 1) % indices.size()];

                if (isConvex(projected[prev], projected[curr], projected[next])) {
                    bool ear = true;
                    for (int j : indices) {
                        if (j == prev || j == curr || j == next) continue;
                        if (isPointInTriangle(projected[j], projected[prev], projected[curr], projected[next])) {
                            ear = false;
                            break;
                        }
                    }
                    if (ear) {
                        triangles.emplace_back(prev, curr, next);
                        indices.erase(indices.begin() + i);
                        earFound = true;
                        edgeTri[{min(prev, next),max(prev, next)}].push_back(triangles.size() - 1);
                        break;
                    }
                }
            }
            if (!earFound) 
                break;
        }
        if ( indices.size() == 3 && isConvex(projected[indices[0]], projected[indices[1]], projected[indices[2]]))
        {
            triangles.emplace_back(indices[0], indices[1], indices[2]);
            edgeTri[{min(indices[0], indices[2]), max(indices[0], indices[2])}].push_back(triangles.size() - 1);
        }
        else if(indices.size() == 3 && !isConvex(projected[indices[0]], projected[indices[1]], projected[indices[2]]))
        {
            triangles.emplace_back(indices[0], indices[2], indices[1]);
            edgeTri[{min(indices[0], indices[1]), max(indices[0], indices[1])}].push_back(triangles.size() - 1);
        }

        for (auto& p : edgeTri)
        {
            if (p.second.size() != 2)
                continue;
            auto [s, e] = p.first;
            auto& firstTris = triangles[p.second[0]];
            auto& secondTris = triangles[p.second[1]];

            int fti = -1, sti = -1;
            tuple_for_each(firstTris,
                [&](auto v)
                {
                    if (v != s && v != e)
                        fti = v;
                });
            tuple_for_each(secondTris,
                [&](auto v)
                {
                    if (v != s && v != e)
                        sti = v;
                });
            if (get<0>(firstTris) == s)
            {
                //fti->e->sti->s
                if (!isConvex(projected[fti], projected[e], projected[sti]) || !isConvex(projected[sti], projected[s], projected[fti]))
                {
                    continue;
                }
            }
            else
            {
                //fti->s->sti->e
                if (!isConvex(projected[fti], projected[s], projected[sti]) || !isConvex(projected[sti], projected[e], projected[fti]))
                {
                    continue;
                }
            }
            if (Float3Dist(vertices[fti], vertices[sti]) < Float3Dist(vertices[s], vertices[e]))
            {
                // e = sti
                get<2>(firstTris) = get<1>(secondTris);
                // 
                get<0>(secondTris) = get<1>(firstTris);
                swap(get<1>(secondTris), get<2>(secondTris));
            }
        }


        return triangles;
    }

    PxVec3 ComputeExternalForce(
        float mass,
        const PxVec3& vPrev,
        const PxVec3& vCurr,
        float deltaTime)
    {
        return mass * (vCurr - vPrev) / deltaTime;
    }

    PxVec3 ComputeExternalTorque(
        const PxMat33& inertiaTensorWorld,
        const PxVec3& wPrev,
        const PxVec3& wCurr,
        float deltaTime)
    {
        PxVec3 dw = (wCurr - wPrev) / deltaTime;
        PxVec3 torque = inertiaTensorWorld * dw + wCurr.cross(inertiaTensorWorld * wCurr);
        return torque;
    }

    PxMat33 LocalinertiaTensorToWorld(PxRigidDynamic* pActor)
    {
        PxVec3 inertiaVec = pActor->getMassSpaceInertiaTensor();
        PxMat33 inertiaLocal(
            PxVec3(inertiaVec.x, 0, 0),
            PxVec3(0, inertiaVec.y, 0),
            PxVec3(0, 0, inertiaVec.z)
        );
        PxMat33 R(pActor->getGlobalPose().q);
        return R * inertiaLocal * R.getTranspose();
    }
}

