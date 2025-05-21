#pragma once
#include "EffectData.h"
#include "Particle_Effect.h"

HRESULT EFFECT_DATA::Make_EffectDatas(shared_ptr<CEffect_Base> In_EffectBase, string strProtoTag)
{

	szPrototypeTag = strProtoTag;

    fEffectTimeMin = In_EffectBase->GetEffectTimeMin();
	fEffectTimeMax = In_EffectBase->GetEffectTimeMax();
	fEffectTimeAccMultiplier = In_EffectBase->GetEffectTimeAccMultiplier();

	vEffectBaseColor = In_EffectBase->GetEffectBaseColor();
	fEffectColorBlendRate = In_EffectBase->GetEffectColorBlendRate();

	vEffectPresetColorR = In_EffectBase->GetEffectColorPreset(0);
	vEffectPresetColorG = In_EffectBase->GetEffectColorPreset(1);
	vEffectPresetColorB = In_EffectBase->GetEffectColorPreset(2);

	bEffectLoop = In_EffectBase->GetEffectLoop();

	bUseGlow = In_EffectBase->GetUseGlow();
	bUseDistortion = In_EffectBase->GetUseDistortion();

	bIsTurn = In_EffectBase->GetEffectIsTurn();

	bUseEffectUvFlowX = In_EffectBase->GetUseEffectUvFlowX();
	bUseEffectUvFlowY = In_EffectBase->GetUseEffectUvFlowY();

	fEffectUvFlowX = In_EffectBase->GetEffectUvFlowX();
	fEffectUvFlowY = In_EffectBase->GetEffectUvFlowY();

	fEffectUvFlowXSpeed = In_EffectBase->GetEffectUvFlowXSpeed();
	fEffectUvFlowYSpeed = In_EffectBase->GetEffectUvFlowYSpeed();

	fEffectUvSizeXMultiplier = In_EffectBase->GetEffectUvSizeXMultiplier();
	fEffectUvSizeYMultiplier = In_EffectBase->GetEffectUvSizeYMultiplier();

	fEffectAlpha = In_EffectBase->GetEffectAlpha();
	bEffectAlphaLinkTimePer = In_EffectBase->GetEffectAlphaLinkTime();

	fTurnSpeed = In_EffectBase->GetTurnSpeed();
    
	_float4x4 tempMatRoot = In_EffectBase->GetRootMatrix();
	memcpy(&matRoot, &tempMatRoot, sizeof(_float4x4));
    
	m_eEffectType = In_EffectBase->GetEffectType();

	if (In_EffectBase->GetEffectType() == CEffect_Base::EFFECT_PARTICLE)
    {
        shared_ptr<CParticle_Effect> pParticleEffect = static_pointer_cast<CParticle_Effect>(In_EffectBase);//In_EffectBase.get());

		vPivot = pParticleEffect->Get_Pivot();
		eInstanceStyle = pParticleEffect->Get_Style();
		vSpeed = pParticleEffect->Get_Speed();
		vRange = pParticleEffect->Get_Range();
		vScale = pParticleEffect->Get_Scale();
		bRandomize = pParticleEffect->Get_IsRandomize();
	}

    return S_OK;
}

void EFFECT_DATA::Bake_Binary(ofstream& os)
{

    write_typed_data(os, szPrototypeTag.size());
    os.write(&szPrototypeTag[0], szPrototypeTag.size());

    write_typed_data(os, fEffectTimeMin);
    write_typed_data(os, fEffectTimeMax);
	write_typed_data(os, fEffectTimeAccMultiplier);
	write_typed_data(os, vEffectBaseColor);
	write_typed_data(os, fEffectColorBlendRate);

	write_typed_data(os, vEffectPresetColorR);
	write_typed_data(os, vEffectPresetColorG);
	write_typed_data(os, vEffectPresetColorB);

	write_typed_data(os, bEffectLoop);

	write_typed_data(os, bUseGlow);
	write_typed_data(os, bUseDistortion);

	write_typed_data(os, bIsTurn);

	write_typed_data(os, bUseEffectUvFlowX);
	write_typed_data(os, bUseEffectUvFlowY);

	write_typed_data(os, fEffectUvFlowX);
	write_typed_data(os, fEffectUvFlowY);

	write_typed_data(os, fEffectUvFlowXSpeed);
	write_typed_data(os, fEffectUvFlowYSpeed);

	write_typed_data(os, fEffectUvSizeXMultiplier);
	write_typed_data(os, fEffectUvSizeYMultiplier);

	write_typed_data(os, fEffectAlpha);
	write_typed_data(os, bEffectAlphaLinkTimePer);

	write_typed_data(os, fTurnSpeed);

	write_typed_data(os, matRoot);

	write_typed_data(os, m_eEffectType);

	//Particle Data
	write_typed_data(os, vPivot);
	write_typed_data(os, eInstanceStyle);
	write_typed_data(os, vSpeed);
	write_typed_data(os, vRange);
	write_typed_data(os, vScale);
	write_typed_data(os, bRandomize);

}

void EFFECT_DATA::Load_FromBinary(ifstream& is)
{

	size_t istringSize;
    read_typed_data(is, istringSize);
	szPrototypeTag.resize(istringSize);
    is.read(&szPrototypeTag[0], istringSize);

	read_typed_data(is, fEffectTimeMin);
	read_typed_data(is, fEffectTimeMax);
	read_typed_data(is, fEffectTimeAccMultiplier);
	read_typed_data(is, vEffectBaseColor);
	read_typed_data(is, fEffectColorBlendRate);

	read_typed_data(is, vEffectPresetColorR);
	read_typed_data(is, vEffectPresetColorG);
	read_typed_data(is, vEffectPresetColorB);

	read_typed_data(is, bEffectLoop);

	read_typed_data(is, bUseGlow);
	read_typed_data(is, bUseDistortion);

	read_typed_data(is, bIsTurn);

	read_typed_data(is, bUseEffectUvFlowX);
	read_typed_data(is, bUseEffectUvFlowY);

	read_typed_data(is, fEffectUvFlowX);
	read_typed_data(is, fEffectUvFlowY);

	read_typed_data(is, fEffectUvFlowXSpeed);
	read_typed_data(is, fEffectUvFlowYSpeed);

	read_typed_data(is, fEffectUvSizeXMultiplier);
	read_typed_data(is, fEffectUvSizeYMultiplier);

	read_typed_data(is, fEffectAlpha);
	read_typed_data(is, bEffectAlphaLinkTimePer);

	read_typed_data(is, fTurnSpeed);

	read_typed_data(is, matRoot);

	read_typed_data(is, m_eEffectType);

	//Particle Data
	read_typed_data(is, vPivot);
	read_typed_data(is, eInstanceStyle);
	read_typed_data(is, vSpeed);
	read_typed_data(is, vRange);
	read_typed_data(is, vScale);
	read_typed_data(is, bRandomize);

}

void EFFECT_DATA::Debug_NodeLog(ostream& os)
{

}