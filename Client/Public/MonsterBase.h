#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMonsterBase abstract : public CGameObject
{
protected:
	CMonsterBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBase(const CMonsterBase& rhs);
public:
	virtual ~CMonsterBase();

public:
	_bool IsBeatAble();
	virtual _bool IsNeedToMove() { return false; };
};

END