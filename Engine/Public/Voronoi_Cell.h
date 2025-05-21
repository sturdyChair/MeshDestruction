#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CVoronoi_Cell
{
public:
	CVoronoi_Cell();
	~CVoronoi_Cell();

public:
	void Set_Particle(const _float3& vParticle);
	void Set_Particle(_fvector vParticle);
	void Calculate_VoronoiCell(const vector<_float3>& Particles);


private:
	// Voronoi Cell
	// Voronoi Cell¿« Vertex List
	vector<_float3> m_Vertices;
	// Voronoi Cell¿« Indices List
	vector<_uint> m_Indices;
	// Voronoi Cell¿« Face List
	vector<_uint> m_Faces;

	_float3 m_vParticle{};
};

END