#include "BoneData.h"

HRESULT BONE_DATA::Make_BoneData(aiBone* In_pAIBone)
{
	memcpy(&OffsetMatrix, &In_pAIBone->mOffsetMatrix, sizeof(_float4x4));
	szName = In_pAIBone->mName.C_Str();
	return S_OK;
}

void BONE_DATA::Bake_Binary(ofstream& os)
{
	write_typed_data(os, szName.size());
	os.write(&szName[0], szName.size());

	write_typed_data(os, OffsetMatrix);

}

void BONE_DATA::Load_FromBinary(ifstream& is)
{
	size_t istringSize;
	read_typed_data(is, istringSize);
	szName.resize(istringSize);
	is.read(&szName[0], istringSize);

	read_typed_data(is, OffsetMatrix);

}