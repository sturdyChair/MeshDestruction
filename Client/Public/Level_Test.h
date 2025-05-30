#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Test : public CLevel
{
private:
	CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_Test();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_Object(const wstring& strLayerTag);
	HRESULT Ready_Layer_Bgm();
	HRESULT Ready_Light();
	void Ready_Ui();
	void Tester();

public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

public:
	static shared_ptr<CLevel_Test> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
