#include "Voronoi_Cell.h"

CVoronoi_Cell::CVoronoi_Cell()
{
}

CVoronoi_Cell::~CVoronoi_Cell()
{
}

void CVoronoi_Cell::Set_Particle(const _float3& vParticle)
{
	Set_Particle(XMLoadFloat3(&vParticle));
}

void CVoronoi_Cell::Set_Particle(_fvector vParticle)
{
	XMStoreFloat3(&m_vParticle, vParticle);
}

void CVoronoi_Cell::Calculate_VoronoiCell(const vector<_float3>& Particles)
{

}
