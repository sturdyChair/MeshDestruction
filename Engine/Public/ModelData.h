#pragma once
#include "Engine_Defines.h"
#include "AnimationData.h"
#include "NodeData.h"
#include "MaterialData.h"

namespace hwiVoro
{
    class container;
}

BEGIN(Engine)

struct ENGINE_DLL MODEL_DATA
{
    Assimp::Importer                    AiImporter;

    shared_ptr<NODE_DATA>               RootNode;

    vector<shared_ptr<MATERIAL_DATA>>   Material_Datas;
    vector<shared_ptr<MESH_DATA>>       Mesh_Datas;
    vector<shared_ptr<ANIMATION_DATA>>  Animation_Datas;

    _float4x4                           TransformMatrix;

    _uint                               iNumMeshs;
    _uint                               iNumMaterials;
    _uint                               iNumAnimations;

    MODEL_TYPE                          eModelType;

    string                              szModelFilePath;
    string                              szModelFileName;

    MESH_VTX_INFO                       VertexInfo;


    HRESULT Make_ModelData(const char* szFilePath, const MODEL_TYPE& eInModelType, _fmatrix TransformMatrix, _bool bAnimZero = false);

    void OnDestroy();
    ~MODEL_DATA();

public:
    HRESULT Make_VoronoiModel(hwiVoro::container& container, const string& szName, const string& szPath);

    void Bake_Binary();

    HRESULT Load_FromAssimp(const _bool bAnimZero);
    void Load_FromBinary();

    HRESULT Load_Anim_Binary(const string& strModelPath);

    void Debug_AnimationLog(ofstream& os);
    void Debug_NonAnimLog(ofstream& os);
    void Debug_MeshLog(ofstream& os);
    void Compute_Center(MESH_VTX_INFO& _tVertexInfo);
    _float Get_MaxOffsetRange() const;
};

END
