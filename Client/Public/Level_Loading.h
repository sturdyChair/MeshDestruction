#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 1. �ε� ������ �����ֱ����� ��ü���� �����ϴ� �۾��� �����Ѵ�. */
/* 2. �ڿ��ε��� ���� �δ���ü�� �������ش�. */
/* Level_Loading�����ϴ� �۾� : ���ν����� */



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