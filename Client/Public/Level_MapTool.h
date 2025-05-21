#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_MapTool : public CLevel
{
private:
	CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_MapTool();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Light();

private:
	void DebugMode();

public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

private:
	_bool m_bDebugMode = false;

public:
	static shared_ptr<CLevel_MapTool> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
