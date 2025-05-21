#pragma once

namespace Engine
{
	enum class PHYSXCOLLIDER_TYPE
	{
		SPHERE,
		BOX,
		CYLINDER,
		CONVEXMESH,
		MESH,
		MODEL,
		CONVEXMODEL,
		BOXWITHPOINTS,
		TYPE_END
	};

	enum class PHYSXACTOR_TYPE
	{
		DYNAMIC,
		YFIXED_DYNAMIC,
		STATIC,
		ZONE,
		TYPE_END
	};

	enum class MODEL_TYPE
	{
		ANIM,
		NONANIM,
		TYPE_END
	};

	enum class MODEL_MOVE_OPTION
	{
		STATIC,
		DYNAMIC,
		TYPE_END
	};
}