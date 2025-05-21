#include "Voronoi_Container.h"

CVoronoi_Container::CVoronoi_Container()
{
}

CVoronoi_Container::~CVoronoi_Container()
{
}

void CVoronoi_Container::Calculate_VoronoiCells(vector<_float3>& particles)
{
	m_Particles = particles;
	m_VoronoiCells.resize(particles.size());
	for (size_t i = 0; i < particles.size(); ++i)
	{
		m_VoronoiCells[i].Set_Particle(particles[i]);
	}
	for (size_t i = 0; i < m_VoronoiCells.size(); ++i)
	{
		m_VoronoiCells[i].Calculate_VoronoiCell(m_Particles);
	}
}
