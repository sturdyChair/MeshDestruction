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
	/* 로딩화면의 배경을 구성하는데 있어 필요한 객체들을 모아놓은 레이어를 생성한다. */
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

public:
	static shared_ptr<CLevel_Logo> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END