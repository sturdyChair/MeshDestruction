#include "AnimationData.h"

HRESULT ANIMATION_DATA::Make_AnimationData(aiAnimation* In_pAiAnimation, _float In_fSpeed)
{
    szName = In_pAiAnimation->mName.data;

    iNumChannels = In_pAiAnimation->mNumChannels;

    fDuration = (_float)In_pAiAnimation->mDuration;
    fTickPerSecond = (_float)In_pAiAnimation->mTicksPerSecond * In_fSpeed;

    for (_uint i(0); i < iNumChannels; i++)
    {
        shared_ptr<CHANNEL_DATA> pChannelData = make_shared<CHANNEL_DATA>();
        pChannelData->Make_ChannelData(In_pAiAnimation->mChannels[i]);
        Channel_Datas.push_back(pChannelData);
    }

    return S_OK;
}

void ANIMATION_DATA::Bake_Binary(const string& strModelFilePath)
{
    _char		szFullPath[MAX_PATH] = {};

    _char		szDrive[MAX_PATH] = {};
    _char		szDir[MAX_PATH] = {};
    _char		szFileName[MAX_PATH] = {};
    _splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

    strcpy_s(szFullPath, szDrive);
    strcat_s(szFullPath, szDir);
    strcat_s(szFullPath, szFileName);
    strcat_s(szFullPath, "/");
    auto strAnimName = szName;
    strAnimName.erase(find(strAnimName.begin(), strAnimName.end(), ':'));
    auto iter = find(strAnimName.begin(), strAnimName.end(), '|');
    if (iter != strAnimName.end())
        strAnimName = string(iter + 1, strAnimName.end());
    strAnimName.append(".bin");
    strcat_s(szFullPath, strAnimName.c_str());

    ofstream os(szFullPath, ios::out | ios::binary);

    write_typed_data(os, szName.size());
    os.write(&szName[0], szName.size());

    write_typed_data(os, iNumChannels);
    write_typed_data(os, fDuration);
    write_typed_data(os, fTickPerSecond);

    for (_uint i = 0; i < iNumChannels; i++)
    {
        Channel_Datas[i]->Bake_Binary(os);
    }

    os.close();
}

void ANIMATION_DATA::Load_FromBinary(ifstream& is, const string& strFolderPath, const string& strFileName)
{
    size_t istringSize;
    read_typed_data(is, istringSize);
    szName.resize(istringSize);
    is.read(&szName[0], istringSize);
    
    // strFileName
    szName = strFileName;
    szName.erase(szName.size() - 4);

    read_typed_data(is, iNumChannels);
    read_typed_data(is, fDuration);
    read_typed_data(is, fTickPerSecond);

    // TODO : !!! DO NOT ERASE !!!
    // Temporary Macro for changing animation speed.
    // undefined after use.
#define SET_ANIM_SPEED(AnimationName, WantedSpeed)\
    if (!strcmp(szName.c_str(), AnimationName))\
    {\
        fTickPerSecond *= WantedSpeed;\
    }

    SET_ANIM_SPEED("Joker_TakeExecution_Start", 1.36f)

        SET_ANIM_SPEED("BossBat_TakeExecution_Start01", 1.006f);

#undef SET_ANIM_SPEED

    for (_uint i(0); i < iNumChannels; i++)
    {
        shared_ptr<CHANNEL_DATA> pChannelData = make_shared<CHANNEL_DATA>();
        pChannelData->Load_FromBinary(is);
        Channel_Datas.push_back(pChannelData);
    }

    // Load additional informations
    string strFilePath = strFolderPath;
    strFilePath.append(szName);
    strFilePath.append(".dat");

    ifstream fileAddInfo(strFilePath, ios::in | ios::binary);
    if (fileAddInfo.is_open())
    {
        read_typed_data(fileAddInfo, isRootAnim);

        _uint iSize = 0;
        read_typed_data(fileAddInfo, iSize);
        for (_uint i = 0; i < iSize; ++i)
        {
            _float fFrame;
            read_typed_data(fileAddInfo, fFrame);
            Callback_Frames.emplace_back(fFrame);
        }
        read_typed_data(fileAddInfo, isForcedNonRootAnim);
    }
    fileAddInfo.close();
}

void ANIMATION_DATA::Bake_ReverseAnimation(shared_ptr<ANIMATION_DATA>& Out_AnimationData)
{
    Out_AnimationData->szName = szName + "_Reverse";

    Out_AnimationData->iNumChannels = iNumChannels;

    Out_AnimationData->fDuration = fDuration;
    Out_AnimationData->fTickPerSecond = fTickPerSecond;

    for (_uint i = 0; i < iNumChannels; i++)
    {
        shared_ptr<CHANNEL_DATA> pChannelData = make_shared<CHANNEL_DATA>();
        Channel_Datas[i]->Bake_ReverseChannel(pChannelData, fDuration);
        Out_AnimationData->Channel_Datas.push_back(pChannelData);
    }
}