#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual ~CLevel_Logo();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* �ε�ȭ���� ����� �����ϴµ� �־� �ʿ��� ��ü���� ��Ƴ��� ���̾ �����Ѵ�. */
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

public:
	static shared_ptr<CLevel_Logo> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END