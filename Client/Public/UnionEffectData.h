#pragma once
#include "Client_Defines.h"
#include "EffectData.h"

struct UNION_EFFECT_DATA
{
	string	    szUnionEffectName = "EffectData";

	_uint 		iNumEffectDatas = 0;

	vector<shared_ptr<EFFECT_DATA>> vEffectDatas;

	HRESULT Make_UnionEffectData(shared_ptr<class CEffect_Base> pEffect, _uint iNum);

public:
	void Bake_Binary(ofstream& os);
	void Load_FromBinary(ifstream& is);

};