#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_AnimTool : public CLevel
{
private:
	CLevel_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_AnimTool();

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
	static shared_ptr<CLevel_AnimTool> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
