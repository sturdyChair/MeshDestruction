#pragma once

#include "Engine_Defines.h"
#include <process.h>
#include <filesystem>

namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC=0, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TEST, LEVEL_TOOL, LEVEL_FIRSTSTAGE, LEVEL_SECONDSTAGE, LEVEL_EFFECTTOOL, LEVEL_ANIMTOOL, LEVEL_MAPTOOL, LEVEL_END };

	struct HIT
	{
		_float4 vHitPoint;
		_float3 vNormal;
		_float3 vDirection;
	};
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

enum class MESH_PASS
{
	Default,
	MIRROR_X,
	AlphaBlend,
	EffectCCW,
	Distortion,
	Slice,
	SliceDissolve,
	Emit,
	Wound,
	EmitColor,
};

enum class EFFECT_PASS
{
	Default,
	AlphaBlend,
	EffectCCW,
	UVFlow,
	Barrier,
	Fire,
};
enum class CUBE_PASS
{
	Default,
	SKY_BOX_1,
	GRID_BOX_2,
	SSD,
	SSD_MONOCOLOR,
};
enum class INSTANCE_PASS
{
	Default,
	HpBar,
	Blend,
	RoundProgress,
	ShakeVertical,
	BlendPoint,
	FromRight,
	FromLeft,
	DecalToDiffuse,
	ParticlePtInstance,
};

enum class EFFECT_TEX_PASS
{
	AlphaBlend,
	WeightBlend,
	WeightBlendMonoColor,
	NonBlendMonoColor,
	UVFlow,
	Flare,
	BlendOriginColor,
	ConvexLens,
	Explosion,
	TextureColor,
	Fire,
};

enum class ANIMMESH_PASS
{
	Default,
	Emission,
	Dissolve,
	MonoColor,
	RimEmission,
	Rim,
	ColorOnly,
	Disintegration,
	Outline,
	Destroy,
	Slice,
	Slice_Dissolve,
	Wound,
	Wound_InnerStencil,
};

using namespace Client;