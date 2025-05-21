#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

BEGIN(Client)
class CResourceWizard
{
public:
	static void LoadResources(const wstring& strProtypeTag, LEVEL level, weak_ptr<CGameInstance> pGameInstance, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
};
END
