#pragma once
#include "UnionEffectData.h"
#include "Effect_Base.h"
#include "Particle_Effect.h"

HRESULT UNION_EFFECT_DATA::Make_UnionEffectData(shared_ptr<class CEffect_Base> pEffect, _uint iNum)
{
	shared_ptr<EFFECT_DATA> In_EffectData = vEffectDatas[iNum];


	pEffect->SetEffectTimeMin(In_EffectData->fEffectTimeMin);
	pEffect->SetEffectTimeMax(In_EffectData->fEffectTimeMax);
	pEffect->SetEffectTimeAccMultiplier(In_EffectData->fEffectTimeAccMultiplier);

	pEffect->SetEffectBaseColorToColor(In_EffectData->vEffectBaseColor);
	pEffect->SetEffectColorBlendRate(In_EffectData->fEffectColorBlendRate);

	pEffect->SetColorPresetR(In_EffectData->vEffectPresetColorR);
	pEffect->SetColorPresetG(In_EffectData->vEffectPresetColorG);
	pEffect->SetColorPresetB(In_EffectData->vEffectPresetColorB);

	pEffect->SetEffectLoop(In_EffectData->bEffectLoop);

	pEffect->SetUseGlow(In_EffectData->bUseGlow);
	pEffect->SetUseDistortion(In_EffectData->bUseDistortion);

	pEffect->SetEffectIsTurn(In_EffectData->bIsTurn);

	pEffect->SetUseEffectUvFlowX(In_EffectData->bUseEffectUvFlowX);
	pEffect->SetUseEffectUvFlowY(In_EffectData->bUseEffectUvFlowY);

	pEffect->SetEffectUvFlowX(In_EffectData->fEffectUvFlowX);
	pEffect->SetEffectUvFlowY(In_EffectData->fEffectUvFlowY);

	pEffect->SetEffectUvFlowXSpeed(In_EffectData->fEffectUvFlowXSpeed);
	pEffect->SetEffectUvFlowYSpeed(In_EffectData->fEffectUvFlowYSpeed);

	pEffect->SetEffectUvSizeXMultiplier(In_EffectData->fEffectUvSizeXMultiplier);
	pEffect->SetEffectUvSizeYMultiplier(In_EffectData->fEffectUvSizeYMultiplier);

	pEffect->SetEffectAlpha(In_EffectData->fEffectAlpha);
	pEffect->SetEffectAlphaLinkTime(In_EffectData->bEffectAlphaLinkTimePer);

	pEffect->SetTurnSpeed(In_EffectData->fTurnSpeed);

	pEffect->SetRootMatrix(In_EffectData->matRoot);

	pEffect->SetEffectType(In_EffectData->m_eEffectType);

	if (In_EffectData->m_eEffectType == CEffect_Base::EFFECT_PARTICLE) {

		shared_ptr<CParticle_Effect> pParticleEffect = static_pointer_cast<CParticle_Effect>(pEffect);

		pParticleEffect->Set_Pivot(In_EffectData->vPivot);
		pParticleEffect->Set_Style(In_EffectData->eInstanceStyle);
		pParticleEffect->Set_Speed(In_EffectData->vSpeed);
		pParticleEffect->Set_Range(In_EffectData->vRange);
		pParticleEffect->Set_Scale(In_EffectData->vScale);
		pParticleEffect->Set_IsRandomize(In_EffectData->bRandomize);
	}

    return S_OK;
}

void UNION_EFFECT_DATA::Bake_Binary(ofstream& os)
{

    write_typed_data(os, szUnionEffectName.size());
    os.write(&szUnionEffectName[0], szUnionEffectName.size());

    write_typed_data(os, iNumEffectDatas);

    for (_uint i = 0; i < vEffectDatas.size(); i++)
    {
        vEffectDatas[i]->Bake_Binary(os);
    }

    os.close();
}

void UNION_EFFECT_DATA::Load_FromBinary(ifstream& is)
{
    size_t istringSize;
    read_typed_data(is, istringSize);
    szUnionEffectName.resize(istringSize);
    is.read(&szUnionEffectName[0], istringSize);

	read_typed_data(is, iNumEffectDatas);

    for (_uint i = 0; i < iNumEffectDatas; i++)
    {
        shared_ptr<EFFECT_DATA> TempEffectData = make_shared<EFFECT_DATA>();
        TempEffectData->Load_FromBinary(is);
        vEffectDatas.push_back(TempEffectData);
    }

}