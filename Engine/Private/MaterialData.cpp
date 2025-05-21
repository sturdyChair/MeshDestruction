#include "MaterialData.h"

HRESULT MATERIAL_DATA::Make_NodeData(aiMaterial* In_pMaterial)
{
    for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        aiString AIStr;

        In_pMaterial->GetTexture(aiTextureType(i), 0, &AIStr);

        szTextureName[i] = AIStr.C_Str();
    }


    return S_OK;
}

void MATERIAL_DATA::Bake_Binary(ofstream& os)
{
    for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        write_typed_data(os, szTextureName[i].size());
        os.write(&szTextureName[i][0], szTextureName[i].size());
    }
}

void MATERIAL_DATA::Load_FromBinary(ifstream& is)
{
    for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        size_t istringSize;
        read_typed_data(is, istringSize);
        szTextureName[i].resize(istringSize);
        is.read(&szTextureName[i][0], istringSize);
    }
}
