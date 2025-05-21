#pragma once
#include "Client_Defines.h"
#include "Effect_Base.h"
#include "VIBuffer_Instance.h"

struct EFFECT_DATA
{
    string	    szPrototypeTag;
    
	//Effect Data
	_float		fEffectTimeMin = 0.f;
	_float		fEffectTimeMax = 1.f;
	_float		fEffectTimeAccMultiplier = 1.f;

	_float4		vEffectBaseColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float      fEffectColorBlendRate = 1.f;

	_float4     vEffectPresetColorR = _float4(1.f, 1.f, 1.f, 1.f);
	_float4     vEffectPresetColorG = _float4(1.f, 1.f, 1.f, 1.f);
	_float4     vEffectPresetColorB = _float4(1.f, 1.f, 1.f, 1.f);

	_bool		bEffectLoop = false;

	_bool		bUseGlow = false;
	_bool		bUseDistortion = false;

	_bool		bIsTurn = false;

	_bool		bUseEffectUvFlowX = false;
	_bool		bUseEffectUvFlowY = false;

	_float		fEffectUvFlowX = 0.f;
	_float		fEffectUvFlowY = 0.f;

	_float		fEffectUvFlowXSpeed = 1.f;
	_float		fEffectUvFlowYSpeed = 1.f;

	_float		fEffectUvSizeXMultiplier = 1.f;
	_float		fEffectUvSizeYMultiplier = 1.f;

	_float		fEffectAlpha = 1.f;
	_bool		bEffectAlphaLinkTimePer = false;

	_float		fTurnSpeed = 1.f;

	_float4x4	matRoot = {};

	CEffect_Base::EFFECT_TYPE m_eEffectType = CEffect_Base::EFFECT_TYPE::EFFECT_NONE;

	//Instance Data

	_float3								vPivot = _float3(0.f, 0.f, 0.f);
	CVIBuffer_Instance::INSTANCE_STYLE	eInstanceStyle = CVIBuffer_Instance::STYLE_NONE;
	_float2								vSpeed = _float2(1.f, 1.f);
	_float3								vRange = _float3(1.f, 1.f, 1.f);
	_float2								vScale = _float2(1.f, 1.f);	
	_bool								bRandomize = false;

	HRESULT Make_EffectDatas(shared_ptr<CEffect_Base> In_EffectBase, string strProtoTag);

public:
    void Bake_Binary(ofstream& os);
    void Load_FromBinary(ifstream& is);

    void Debug_NodeLog(ostream& os);

};