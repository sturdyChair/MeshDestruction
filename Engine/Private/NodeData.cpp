#include "NodeData.h"

HRESULT NODE_DATA::Make_NodeData(aiNode* In_pAINode)
{
    memcpy(&TransformationMatrix, &In_pAINode->mTransformation, sizeof(_float4x4));
    szName = In_pAINode->mName.C_Str();
    iNumChildren = In_pAINode->mNumChildren;

    for (_uint i = 0; i < iNumChildren; i++)
    {
        shared_ptr<NODE_DATA> ChildNode = make_shared<NODE_DATA>();
        ChildNode->Make_NodeData(In_pAINode->mChildren[i]);
        pChildren.push_back(ChildNode);
    }

    return S_OK;
}

HRESULT NODE_DATA::Make_VoronoiNodeData(_uint iNumMesh)
{

    return E_NOTIMPL;
}

void NODE_DATA::Bake_Binary(ofstream& os)
{

    write_typed_data(os, szName.size());
    os.write(&szName[0], szName.size());

    write_typed_data(os, TransformationMatrix);
    write_typed_data(os, iNumChildren);

    for (_uint i = 0; i < iNumChildren; i++)
    {
        pChildren[i]->Bake_Binary(os);
    }

}

void NODE_DATA::Load_FromBinary(ifstream& is)
{
    size_t istringSize;
    read_typed_data(is, istringSize);
    szName.resize(istringSize);
    is.read(&szName[0], istringSize);

    read_typed_data(is, TransformationMatrix);
    read_typed_data(is, iNumChildren);

    for (_uint i = 0; i < iNumChildren; i++)
    {
        shared_ptr<NODE_DATA> ChildNode = make_shared<NODE_DATA>();
        ChildNode->Load_FromBinary(is);
        pChildren.push_back(ChildNode);
    }

}

void NODE_DATA::Debug_NodeLog(ostream& os)
{
    os << szName << endl;

    for (_uint i = 0; i < iNumChildren; i++)
    {
        pChildren[i]->Debug_NodeLog(os);
    }

}