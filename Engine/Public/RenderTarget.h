
#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CRenderTarget
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CRenderTarget();

public:
	ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }
	ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }
	ID3D11Texture2D* Get_Texture() const { return m_pTexture2D; }
	HRESULT						Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName);


#ifdef _DEBUG
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT	Render_Debug(shared_ptr<class CShader> pShader, const _char* pConstantName, shared_ptr<class CVIBuffer_Rect> pVIBuffer);

#endif

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	void Clear();
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ID3D11Texture2D* m_pTexture2D = { nullptr };

	ID3D11ShaderResourceView* m_pSRV = { nullptr };

	ID3D11RenderTargetView* m_pRTV = { nullptr };

	_float4							m_vClearColor = {};

	_float4x4 m_WorldMatrix{};

public:
	static shared_ptr<CRenderTarget> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	void Free();

};

END