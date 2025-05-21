#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CLight_Manager
{
private:
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CLight_Manager();

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;

public:
	HRESULT Initialize();
	HRESULT TickEngine(_float fTimeDelta);
	HRESULT Add_Light(void* LightDesc);
	shared_ptr<class CLight> Add_Light_Return(void* LightDesc);
	//HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Bind_Lights(shared_ptr<class CShader> pShader);

	void LightControl(string strLightTag, _bool bOn);
	void LightControl(string strLightTag);

	void LightClear();

	void LightDeleteCheck();
private:
	list<shared_ptr<class CLight>>			m_Lights;

	_uint m_iLightCount = 0;

	vector<PointLight>			m_pointLights;
	vector<DirectionalLight>	m_directionalLights;
	vector<SpotLight>			m_spotLights;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

public:
	static shared_ptr<CLight_Manager> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();

};

END