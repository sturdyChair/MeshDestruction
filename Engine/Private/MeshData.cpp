#pragma once
#include "MeshData.h"

HRESULT MESH_DATA::Make_MeshData(const MODEL_TYPE& In_eModelType, aiMesh* In_pAiMesh, const _fmatrix& In_TransformMatrix, MESH_VTX_INFO* In_pVertexInfo)
{
    szName = In_pAiMesh->mName.C_Str();
    eModelType = In_eModelType;
    iNumVertices = In_pAiMesh->mNumVertices;
    iNumFaces = In_pAiMesh->mNumFaces;
    iNumBones = In_pAiMesh->mNumBones;
    iMaterialIndex = In_pAiMesh->mMaterialIndex;

    // 애니메이션
    if (MODEL_TYPE::ANIM == In_eModelType)
    {
        for (_uint i(0); i < iNumBones; i++)
        {
            shared_ptr<BONE_DATA> pBoneData = make_shared<BONE_DATA>();
            pBoneData->Make_BoneData(In_pAiMesh->mBones[i]);
            Bone_Datas.push_back(pBoneData);
        }

        pAnimVertices = shared_ptr<VTXANIM[]>(DBG_NEW VTXANIM[iNumVertices]);

        for (_uint i(0); i < iNumVertices; ++i)
        {
            memcpy(&pAnimVertices[i].vPosition, &In_pAiMesh->mVertices[i], sizeof(_float3));
            //XMStoreFloat3(&pAnimVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pAnimVertices[i].vPosition), In_TransformMatrix));

            memcpy(&pAnimVertices[i].vNormal, &In_pAiMesh->mNormals[i], sizeof(_float3));
            XMStoreFloat3(&pAnimVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pAnimVertices[i].vNormal), In_TransformMatrix)));

            memcpy(&pAnimVertices[i].vTexcoord, &In_pAiMesh->mTextureCoords[0][i], sizeof(_float2));
            memcpy(&pAnimVertices[i].vTangent, &In_pAiMesh->mTangents[i], sizeof(_float3));

            Check_Position(In_pVertexInfo->vMax, pAnimVertices[i].vPosition, [](_float _fLeft, _float _fRight)->_bool { return _fLeft < _fRight; });
            Check_Position(In_pVertexInfo->vMin, pAnimVertices[i].vPosition, [](_float _fLeft, _float _fRight)->_bool { return _fLeft > _fRight; });
        }

        for (_uint i(0); i < iNumBones; ++i)
        {
            aiBone* pAIBone = In_pAiMesh->mBones[i];

            /* pAIBone->mNumWeights : 이 뼈는 몇개의 정점에 영향릉 주는지 */
            for (_uint j(0); j < pAIBone->mNumWeights; ++j)
            {
                /* pAIBone->mWeights[j].mVertexId : 그 중에 j번째 정점의 인덱스는 뭐였는지?  */
                /* pAIBone->mWeights[j].mWeight : j번째 정점에게 적용해야할 가중치. */

                if (0 == pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x)
                {
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.x = i;
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
                }

                else if (0 == pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y)
                {
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.y = i;
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
                }

                else if (0 == pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z)
                {
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.z = i;
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
                }

                else
                {
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.w = i;
                    pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
                }
            }
        }

    }

    // 애니메이션이 아님
    else if (MODEL_TYPE::NONANIM == In_eModelType)
    {
        pVertices = shared_ptr<VTXMESH[]>(DBG_NEW VTXMESH[iNumVertices]);
        pPosVertices = shared_ptr<VTXPOS[]>(DBG_NEW VTXPOS[iNumVertices]);

        for (_uint i(0); i < iNumVertices; ++i)
        {
            memcpy(&pVertices[i].vPosition, &In_pAiMesh->mVertices[i], sizeof(_float3));
            XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), In_TransformMatrix));
            memcpy(&pPosVertices[i].vPosition, &pVertices[i].vPosition, sizeof(_float3));

            memcpy(&pVertices[i].vNormal, &In_pAiMesh->mNormals[i], sizeof(_float3));
            XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), In_TransformMatrix)));

            if (In_pAiMesh->mTextureCoords[0])
                memcpy(&pVertices[i].vTexcoord, &In_pAiMesh->mTextureCoords[0][i], sizeof(_float2));
            else
                pVertices[i].vTexcoord = { 0.f, 0.f };

            if (In_pAiMesh->mTangents)
                memcpy(&pVertices[i].vTangent, &In_pAiMesh->mTangents[i], sizeof(_float3));
            else
                pVertices[i].vTangent = { 0.f, 0.f, 0.f };

            Check_Position(In_pVertexInfo->vMax, pVertices[i].vPosition, [](_float _fLeft, _float _fRight)->_bool { return _fLeft < _fRight; });
            Check_Position(In_pVertexInfo->vMin, pVertices[i].vPosition, [](_float _fLeft, _float _fRight)->_bool { return _fLeft > _fRight; });
        }
    }

    else
    {
        //모델 타입이 지정되지 않음.
        DEBUG_ASSERT;
    }

    pIndices = shared_ptr<FACEINDICES32[]>(DBG_NEW FACEINDICES32[iNumFaces]);

    for (_uint i(0); i < iNumFaces; ++i)
    {
        pIndices[i]._1 = In_pAiMesh->mFaces[i].mIndices[0];
        pIndices[i]._2 = In_pAiMesh->mFaces[i].mIndices[1];
        pIndices[i]._3 = In_pAiMesh->mFaces[i].mIndices[2];
        pIndices[i].numIndices = In_pAiMesh->mFaces[i].mNumIndices;
    }

    return S_OK;
}

HRESULT MESH_DATA::Make_VCMesh(vector<_float3>& vertices, vector<_float3>& normals, vector<_uint>& indicies, vector<_float2>& texCoords,
    string _szName, _uint iMatIdx, _uint iBoneIdx, MESH_VTX_INFO& Info, const _fmatrix& In_TransformMatrix)
{//On Implementation
    szName = _szName;
    eModelType = MODEL_TYPE::NONANIM;
    iNumVertices = vertices.size();
    iNumFaces = indicies.size() / 3;
    iNumBones = 1;
    iMaterialIndex = iMatIdx;

    //Info.vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    //Info.vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
    for (_uint i(0); i < iNumBones; i++)
    {//ON IMPL
        shared_ptr<BONE_DATA> pBoneData = make_shared<BONE_DATA>();
        //pBoneData->Make_BoneData(In_pAiMesh->mBones[i]);
        Bone_Datas.push_back(pBoneData);
    }

    pAnimVertices = shared_ptr<VTXANIM[]>(DBG_NEW VTXANIM[iNumVertices]);
    pVertices = shared_ptr<VTXMESH[]>(DBG_NEW VTXMESH[iNumVertices]);

    for (_uint i(0); i < iNumVertices; ++i)
    {
        memcpy(&pAnimVertices[i].vPosition, &vertices[i], sizeof(_float3));
        memcpy(&pVertices[i].vPosition, &vertices[i], sizeof(_float3));

        memcpy(&pAnimVertices[i].vNormal, &normals[i], sizeof(_float3));
        memcpy(&pVertices[i].vNormal, &normals[i], sizeof(_float3));

        XMStoreFloat3(&pAnimVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pAnimVertices[i].vNormal), In_TransformMatrix)));
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), In_TransformMatrix)));

        //memcpy(&pAnimVertices[i].vTexcoord, &texCoords[i], sizeof(_float2));
        //memcpy(&pVertices[i].vTexcoord, &texCoords[i], sizeof(_float2));
        pAnimVertices[i].vBlendIndex.x = iBoneIdx;
        pAnimVertices[i].vBlendWeight.x = 1.f;

        //memcpy(&pAnimVertices[i].vTangent, &In_pAiMesh->mTangents[i], sizeof(_float3));

        Check_Position(Info.vMax, pAnimVertices[i].vPosition, [](_float _fLeft, _float _fRight)->_bool { return _fLeft < _fRight; });
        Check_Position(Info.vMin, pAnimVertices[i].vPosition, [](_float _fLeft, _float _fRight)->_bool { return _fLeft > _fRight; });
    }

    _float fRangeX = Info.vMax.x - Info.vMin.x;
    _float fRangeY = Info.vMax.y - Info.vMin.y;
    _float fRangeZ = Info.vMax.z - Info.vMin.z;

    for (_uint i(0); i < iNumVertices; ++i)
    {
        if (fabsf(pAnimVertices[i].vNormal.y) > max(fabsf(pAnimVertices[i].vNormal.x), fabsf(pAnimVertices[i].vNormal.z)))
        {
            pAnimVertices[i].vTexcoord.x = (pAnimVertices[i].vPosition.x - Info.vMin.x) / fRangeX;
		    pAnimVertices[i].vTexcoord.y = (pAnimVertices[i].vPosition.z - Info.vMin.z) / fRangeZ;

	    }
        else
        {
            if (fabsf(pAnimVertices[i].vNormal.z) > max(fabsf(pAnimVertices[i].vNormal.x), fabsf(pAnimVertices[i].vNormal.y)))
            {
			    pAnimVertices[i].vTexcoord.x = (pAnimVertices[i].vPosition.x - Info.vMin.x) / fRangeX;
			    pAnimVertices[i].vTexcoord.y = (pAnimVertices[i].vPosition.y - Info.vMin.y) / fRangeY;
            }
            else
            {
			    pAnimVertices[i].vTexcoord.x = (pAnimVertices[i].vPosition.z - Info.vMin.z) / fRangeZ;
			    pAnimVertices[i].vTexcoord.y = (pAnimVertices[i].vPosition.y - Info.vMin.y) / fRangeY;
            }
        }
        pVertices[i].vTexcoord.x = pAnimVertices[i].vTexcoord.x;
        pVertices[i].vTexcoord.y = pAnimVertices[i].vTexcoord.y;
    }

    pIndices = shared_ptr<FACEINDICES32[]>(DBG_NEW FACEINDICES32[iNumFaces]);
	vector<_vector> Tangents(iNumVertices, { 0.f, 0.f, 0.f, 0.f });;
    for (_uint i(0); i < iNumFaces; ++i)
    {
        pIndices[i]._1 = indicies[i * 3];
        pIndices[i]._2 = indicies[i * 3 + 1];
        pIndices[i]._3 = indicies[i * 3 + 2];
        pIndices[i].numIndices = 3;
        //calc tangent
        array<_float3, 3> vTangentArray{};
		array<_float3, 3> vPosArray{ pAnimVertices[pIndices[i]._1].vPosition, pAnimVertices[pIndices[i]._2].vPosition, pAnimVertices[pIndices[i]._3].vPosition };
		array<_float2, 3> vTexcoordArray{ pAnimVertices[pIndices[i]._1].vTexcoord, pAnimVertices[pIndices[i]._2].vTexcoord, pAnimVertices[pIndices[i]._3].vTexcoord };
        for (_uint i = 0; i < 3; ++i)
        {
            _vector vEdge1 = XMLoadFloat3(&vPosArray[(i + 1) % 3]) - XMLoadFloat3(&vPosArray[i]);
			_vector vEdge2 = XMLoadFloat3(&vPosArray[(i + 2) % 3]) - XMLoadFloat3(&vPosArray[i]);
			_vector vDeltaUV1 = XMLoadFloat2(&vTexcoordArray[(i + 1) % 3]) - XMLoadFloat2(&vTexcoordArray[i]);
			_vector vDeltaUV2 = XMLoadFloat2(&vTexcoordArray[(i + 2) % 3]) - XMLoadFloat2(&vTexcoordArray[i]);
			_float fDet = XMVectorGetX(vDeltaUV1) * XMVectorGetY(vDeltaUV2) - XMVectorGetY(vDeltaUV1) * XMVectorGetX(vDeltaUV2);
            if (fabsf(fDet) > 0.0001f)
            {
                _float fInvDet = 1.f / fDet;
                vTangentArray[i].x = fInvDet * (XMVectorGetY(vDeltaUV2) * XMVectorGetX(vEdge1) - XMVectorGetY(vDeltaUV1) * XMVectorGetX(vEdge2));
                vTangentArray[i].y = fInvDet * (XMVectorGetY(vDeltaUV2) * XMVectorGetY(vEdge1) - XMVectorGetY(vDeltaUV1) * XMVectorGetY(vEdge2));
                vTangentArray[i].z = fInvDet * (XMVectorGetY(vDeltaUV2) * XMVectorGetZ(vEdge1) - XMVectorGetY(vDeltaUV1) * XMVectorGetZ(vEdge2));
            }

        }
        Tangents[pIndices[i]._1] += XMVector3Normalize(XMLoadFloat3(&vTangentArray[0]));
        Tangents[pIndices[i]._2] += XMVector3Normalize(XMLoadFloat3(&vTangentArray[1]));
        Tangents[pIndices[i]._3] += XMVector3Normalize(XMLoadFloat3(&vTangentArray[2]));
    }
	for (_uint i(0); i < iNumVertices; ++i)
	{
        XMStoreFloat3(&pAnimVertices[i].vTangent, XMVector3Normalize(Tangents[i]));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3Normalize(Tangents[i]));
	}


    return S_OK;
}

void MESH_DATA::Bake_Binary(ofstream& os)
{
    write_typed_data(os, szName.size());
    os.write(&szName[0], szName.size());

    write_typed_data(os, eModelType);
    write_typed_data(os, iNumVertices);
    write_typed_data(os, iNumFaces);
    write_typed_data(os, iNumBones);
    write_typed_data(os, iMaterialIndex);

    if (MODEL_TYPE::ANIM == eModelType)
    {
        for (_uint i(0); i < iNumBones; i++)
            Bone_Datas[i]->Bake_Binary(os);

        for (_uint i(0); i < iNumVertices; ++i)
            write_typed_data(os, pAnimVertices[i]);
    }

    else if (MODEL_TYPE::NONANIM == eModelType)
    {
        for (_uint i(0); i < iNumVertices; ++i)
            write_typed_data(os, pVertices[i]);
    }

    for (_uint i(0); i < iNumFaces; ++i)
        write_typed_data(os, pIndices[i]);

}

void MESH_DATA::Load_FromBinary(ifstream& is)
{
    size_t istringSize;
    read_typed_data(is, istringSize);
    szName.resize(istringSize);
    is.read(&szName[0], istringSize);

    read_typed_data(is, eModelType);
    read_typed_data(is, iNumVertices);
    read_typed_data(is, iNumFaces);
    read_typed_data(is, iNumBones);
    read_typed_data(is, iMaterialIndex);

    if (MODEL_TYPE::ANIM == eModelType)
    {
        for (_uint i(0); i < iNumBones; i++)
        {
            shared_ptr<BONE_DATA> pBoneData = make_shared<BONE_DATA>();
            pBoneData->Load_FromBinary(is);
            Bone_Datas.push_back(pBoneData);
        }

        pAnimVertices = shared_ptr<VTXANIM[]>(DBG_NEW VTXANIM[iNumVertices]);

        for (_uint i(0); i < iNumVertices; ++i)
        {
            read_typed_data(is, pAnimVertices[i]);
        }

    }

    else if (MODEL_TYPE::NONANIM == eModelType)
    {
        pVertices = shared_ptr<VTXMESH[]>(DBG_NEW VTXMESH[iNumVertices]);
        pPosVertices = shared_ptr<VTXPOS[]>(DBG_NEW VTXPOS[iNumVertices]);

        for (_uint i(0); i < iNumVertices; ++i)
        {
            read_typed_data(is, pVertices[i]);
            memcpy(&pPosVertices[i].vPosition, &pVertices[i].vPosition, sizeof(_float3));
        }
    }

    pIndices = shared_ptr<FACEINDICES32[]>(DBG_NEW FACEINDICES32[iNumFaces]);

    for (_uint i(0); i < iNumFaces; ++i)
    {
        read_typed_data(is, pIndices[i]);
    }

}

void MESH_DATA::Check_Position(_float3& _vRecodePosion, _float3 _vValue, _bool(*_pFncPtr)(_float, _float))
{
    if (_pFncPtr(_vRecodePosion.x, _vValue.x))
        _vRecodePosion.x = _vValue.x;

    if (_pFncPtr(_vRecodePosion.y, _vValue.y))
        _vRecodePosion.y = _vValue.y;

    if (_pFncPtr(_vRecodePosion.z, _vValue.z))
        _vRecodePosion.z = _vValue.z;
}