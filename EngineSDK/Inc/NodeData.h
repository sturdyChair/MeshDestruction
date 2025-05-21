#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

struct ENGINE_DLL NODE_DATA
{
    string			                    szName;
    _uint                               iNumChildren;

    _float4x4                           TransformationMatrix;

    vector<shared_ptr<NODE_DATA>>       pChildren;

    HRESULT Make_NodeData(aiNode* In_pAINode);
    HRESULT Make_VoronoiNodeData(_uint iNumMesh);

public:

    void Bake_Binary(ofstream& os);
    void Load_FromBinary(ifstream& is);

    void Debug_NodeLog(ostream& os);

};

END