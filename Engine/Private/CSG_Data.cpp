#include "CSG_Data.h"
#include "CSG_Manager.h"

BEGIN(Engine)

void BSP_Node::MakePolygon(const CSG_Mesh& mesh)
{
	polygon.Clear();
	polygon = CCSG_Manager::MakePolygon(mesh, coplanar);

}


END