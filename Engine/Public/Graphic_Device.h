#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)
class CGraphic_Device
{
private:
	CGraphic_Device();
public:
	virtual ~CGraphic_Device();

public:

	/* 그래픽 디바이스의 초기화. */
	HRESULT Initialize(const ENGINE_DESC& EngineDesc,
		_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);

	/* 백버퍼를 지운다. */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_RTVs(_float4 vClearColor);


	HRESULT Bind_BackBuffer();
	HRESULT Bind_RTVs();

	/* 깊이버퍼 + 스텐실버퍼를 지운다. */
	HRESULT Clear_DepthStencil_View();

	/* 후면 버퍼를 전면버퍼로 교체한다.(백버퍼를 화면에 직접 보여준다.) */
	HRESULT Present();

	void Map_Picking_Screen();

	_uint Pick_Screen(_uint iX, _uint iY);

	void Unmap_Picking_Screen();

	void Map_Picking_Position();

	_float4 Pick_Position(_uint iX, _uint iY);

	void Unmap_Picking_Position();


	HRESULT Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag);

	HRESULT Bind_RT_SRV(const _wstring& strRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName);

	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool bClear);

	HRESULT End_MRT();
	shared_ptr<class CRenderTarget> Find_RenderTarget(const _wstring& strRenderTargetTag);
	list<shared_ptr<class CRenderTarget>>* Find_MRT(const _wstring& strMRTTag);

	HRESULT Bind_Default_ViewPort();

#ifdef _DEBUG
	HRESULT Ready_Debug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _wstring& strMRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName, shared_ptr< class CVIBuffer_Rect> pVIBuffer);
#endif


private:

	ID3D11Device* m_pDevice = { nullptr };

	/* 기능실행.(바인딩작업, 정점버퍼를 SetVertexBuffers(), SetIndexBuffer(), Apply() */
	/* 그린다. */
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	/* 후면버퍼와 전면버퍼를 교체해가면서 화면에 보여주는 역할 */
	IDXGISwapChain* m_pSwapChain = { nullptr };

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };

	ID3D11Texture2D* m_pHitScreenTexture = { nullptr };
	ID3D11RenderTargetView* m_pHitScreenRTV = { nullptr };
	ID3D11Texture2D* m_pDepthTexture = { nullptr };

	ID3D11Texture2D* m_pStagingTexture = nullptr;
	D3D11_MAPPED_SUBRESOURCE	m_mappedResource;

	ID3D11Texture2D* m_pStagingPosition = nullptr;
	D3D11_MAPPED_SUBRESOURCE	m_mappedPosition;

	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };
	//ID3D11DepthStencilView*		m_pDepthStencilViewQuarter = { nullptr };

private:
	HRESULT Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

	HRESULT Ready_RenderTargets(_uint iWinCX, _uint iWinCY);

private:
	_uint	m_iWinSizeX = 0;
	_uint	m_iWinSizeY = 0;

private:
	map<const _wstring, shared_ptr<class CRenderTarget>>			m_RenderTargets;

private:
	map<const _wstring, list<shared_ptr<class CRenderTarget>>>		m_MRTs;

private:
	D3D11_VIEWPORT m_ViewPort;
public:
	static const _wstring strObjectIdRenderTargetTag;
	static const _wstring strPositionRenderTargetTag;
	static const _wstring strAlbedoRenderTargetTag;
	static const _wstring strNormalRenderTargetTag;
	static const _wstring strOrmRenderTargetTag;
	static const _wstring strEmissionRenderTargetTag;
	static const _wstring strDistortionRenderTargetTag;

	static const _wstring strWeightedBlendedTargetTag;


	static const _wstring strGlowRenderTargetTag;
	static const _wstring strGlowBlurRenderTargetTag;
	static const _wstring strGlowPowerRenderTargetTag;

	static const _wstring strMRT_BeforeBack_Tag;
	static const _wstring strMRT_Distortion_Tag;
	static const _wstring strMRT_D_N_ORM_Tag;


	static const _wstring strMRT_WeightedBlended_Tag;

	static const _wstring strMRT_Glow_Tag;
	static const _wstring strMRT_GlowBlur_Tag;
	static const _wstring strMRT_GlowPower_Tag;



	static const _wstring strMRT_CrossPower_Tag;
	static const _wstring strMRT_Cross_Tag;

	static const _wstring strBeforeBackTarget;
	static const _wstring strCrossPowerTarget;
public:
	static shared_ptr<CGraphic_Device> Create(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice,
		_Out_ ID3D11DeviceContext** ppDeviceContextOut);

	void Free();



};
END
