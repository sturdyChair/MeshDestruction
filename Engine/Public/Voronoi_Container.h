#pragma once
#include "Engine_Defines.h"
#include "Voronoi_Cell.h"

BEGIN(Engine)

class CVoronoi_Container
{
public:
	CVoronoi_Container();
	~CVoronoi_Container();

	void Calculate_VoronoiCells(vector<_float3>& particles);


private:
	vector<CVoronoi_Cell> m_VoronoiCells;
	vector<_float3> m_Particles;
	_float3 m_vMin{ -1.f,-1.f,-1.f };
	_float3 m_vMax{ 1.f,1.f,1.f };
};

END