#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 1. 로딩 레벨을 보여주기위한 객체들을 생성하는 작업을 수행한다. */
/* 2. 자원로딩을 위한 로더객체를 생성해준다. */
/* Level_Loading생성하는 작업 : 메인스레드 */



BEGIN(Client)

class CLevel_Loading : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_Loading();

public:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Ui(const wstring& strLayerTag);

private:
	LEVEL						m_eNextLevelID = { LEVEL_END };
	shared_ptr<class CLoader>   m_pLoader = { nullptr };

public:
	static shared_ptr<CLevel_Loading> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	void Free();
};

END