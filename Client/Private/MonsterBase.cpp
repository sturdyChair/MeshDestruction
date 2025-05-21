#include "MonsterBase.h"
#include "GameInstance.h"

#include "Beat_Manager.h"


CMonsterBase::CMonsterBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
	: CGameObject{ rhs }
{
}

CMonsterBase::~CMonsterBase()
{
}

_bool CMonsterBase::IsBeatAble()
{
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float fBeatTime = CBeat_Manager::Get_Instance()->Get_Timer();
    return fBeatTime < 0.01f || fBeatTime > fBeat - 0.01f;
}
