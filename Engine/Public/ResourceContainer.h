#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CResource_Container
{

private:
	CResource_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CResource_Container();


public:
	void Initialize();
	void Tick(_float fTimeDelta);
	void Clear();
	void Render();

	shared_ptr<CTexture> SearchMaterial(const _wstring& pMaterialTag);

private:
	map<const _wstring, shared_ptr<class CTexture>> m_MaterialList;

public:
	static shared_ptr<CResource_Container> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
};

END