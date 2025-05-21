#include "..\public\Graphic_Device.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "NvFlowExt.h"
#include "NvFlowLoader.h"


//const _wstring CGraphic_Device::strCrossUpTarget	= L"RT_CrossUp";
//const _wstring CGraphic_Device::strCrossRightTarget = L"RT_CrossRight";
const _wstring CGraphic_Device::strObjectIdRenderTargetTag = L"RT_Id";
const _wstring CGraphic_Device::strPositionRenderTargetTag = L"RT_Position";
const _wstring CGraphic_Device::strAlbedoRenderTargetTag = L"RT_Albedo";
const _wstring CGraphic_Device::strNormalRenderTargetTag = L"RT_Normal";
const _wstring CGraphic_Device::strOrmRenderTargetTag = L"RT_ORM";
const _wstring CGraphic_Device::strEmissionRenderTargetTag = L"RT_Emission";


const _wstring CGraphic_Device::strWeightedBlendedTargetTag = L"RT_WeightedBlended";


const _wstring CGraphic_Device::strGlowRenderTargetTag = L"RT_Glow";
const _wstring CGraphic_Device::strGlowPowerRenderTargetTag = L"RT_GlowPower";
const _wstring CGraphic_Device::strGlowBlurRenderTargetTag = L"RT_GlowBlur";


const _wstring CGraphic_Device::strMRT_D_N_ORM_Tag = L"MRT_DEFFERED";

const _wstring CGraphic_Device::strMRT_Glow_Tag = L"MRT_GLOW";
const _wstring CGraphic_Device::strMRT_GlowBlur_Tag = L"MRT_GLOWBLUR";
const _wstring CGraphic_Device::strMRT_GlowPower_Tag = L"MRT_GLOWPOWER";

const _wstring CGraphic_Device::strMRT_BeforeBack_Tag = L"MRT_BeforeBack";
const _wstring CGraphic_Device::strMRT_Cross_Tag = L"MRT_Cross";
const _wstring CGraphic_Device::strMRT_CrossPower_Tag = L"MRT_CrossPower";
const _wstring CGraphic_Device::strMRT_Distortion_Tag = L"MRT_Distortion";

const _wstring CGraphic_Device::strMRT_WeightedBlended_Tag = L"MRT_WeightedBlended";

const _wstring CGraphic_Device::strBeforeBackTarget = L"RT_BeforeBack";
const _wstring CGraphic_Device::strDistortionRenderTargetTag = L"RT_Distortion";
const _wstring CGraphic_Device::strCrossPowerTarget = L"RT_CrossPower";

CGraphic_Device::CGraphic_Device()
	: m_pDevice{ nullptr }
	, m_pDeviceContext{ nullptr }
{

}

CGraphic_Device::~CGraphic_Device()
{
	Free();
}

HRESULT CGraphic_Device::Initialize(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FeatureLV;

	/* 그래픽 장치를 초기화한다. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;

	/* SwapChain 전면과 후면버퍼를 번갈아가며 화면에 보여준다.(Present) */

	/* 백버퍼를 생성하기 위한 texture2D 만든거야. */
	if (FAILED(Ready_SwapChain(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY)))
		return E_FAIL;

	/* 스왑체인이 들고 있는 텍스쳐 2D를 가져와서 이를 바탕으로 백버퍼 렌더타겟 뷰를 만든다.*/
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(EngineDesc.iWinSizeX, EngineDesc.iWinSizeY)))
		return E_FAIL;

	if (FAILED(Ready_RenderTargets(EngineDesc.iWinSizeX, EngineDesc.iWinSizeY)))
		return E_FAIL;


	Add_MRT(strMRT_D_N_ORM_Tag, strAlbedoRenderTargetTag);
	Add_MRT(strMRT_D_N_ORM_Tag, strObjectIdRenderTargetTag);
	Add_MRT(strMRT_D_N_ORM_Tag, strPositionRenderTargetTag);
	Add_MRT(strMRT_D_N_ORM_Tag, strNormalRenderTargetTag);
	Add_MRT(strMRT_D_N_ORM_Tag, strOrmRenderTargetTag);
	Add_MRT(strMRT_D_N_ORM_Tag, strEmissionRenderTargetTag);

	Add_MRT(strMRT_Glow_Tag, strGlowRenderTargetTag);
	//Add_MRT(strMRT_Glow_Tag, strObjectIdRenderTargetTag);
	Add_MRT(strMRT_GlowBlur_Tag, strGlowBlurRenderTargetTag);
	Add_MRT(strMRT_GlowPower_Tag, strGlowPowerRenderTargetTag);

	Add_MRT(strMRT_BeforeBack_Tag, strBeforeBackTarget);
	//Add_MRT(strMRT_BeforeBack_Tag, strObjectIdRenderTargetTag);

	Add_MRT(strMRT_CrossPower_Tag, strCrossPowerTarget);


	Add_MRT(strMRT_Distortion_Tag, strDistortionRenderTargetTag);




	/* 장치에 바인드해놓을 렌더타겟들과 뎁스스텐실뷰를 세팅한다. */
	/* 장치는 동시에 최대 8개의 렌더타겟을 들고 있을 수 있다. */
	ID3D11RenderTargetView* pRTVs[1] = {
		m_pBackBufferRTV,
	};

	m_pDeviceContext->OMSetRenderTargets(1, pRTVs,
		m_pDepthStencilView);

	ZeroMemory(&m_ViewPort, sizeof(D3D11_VIEWPORT));
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = (_float)EngineDesc.iWinSizeX;
	m_ViewPort.Height = (_float)EngineDesc.iWinSizeY;
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &m_ViewPort);

	*ppDevice = m_pDevice;
	*ppContext = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	m_iWinSizeX = EngineDesc.iWinSizeX;
	m_iWinSizeY = EngineDesc.iWinSizeY;




	
	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 백버퍼를 초기화한다.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);


	return S_OK;
}

HRESULT CGraphic_Device::Clear_RTVs(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;


	return E_NOTIMPL;
}

HRESULT CGraphic_Device::Bind_BackBuffer()
{
	ID3D11RenderTargetView* pRTVs[1] = {
		m_pBackBufferRTV,
	};

	m_pDeviceContext->OMSetRenderTargets(1, pRTVs,
		m_pDepthStencilView);
	return S_OK;
}

HRESULT CGraphic_Device::Bind_RTVs()
{
	//ID3D11RenderTargetView* pRTVs[5] = {
	//	m_pAlbedoRTV,
	//	m_pNormalRTV,
	//	m_pOrmRTV,
	//	m_pPositionRTV,
	//	m_pIDRTV,
	//};

	//m_pDeviceContext->OMSetRenderTargets(5, pRTVs,
	//	m_pDepthStencilView);
	return E_NOTIMPL;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	//m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilViewQuarter, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	/* 전면 버퍼와 후면 버퍼를 교체하여 후면 버퍼를 전면으로 보여주는 역할을 한다. */
	/* 후면 버퍼를 직접 화면에 보여줄게. */
	return m_pSwapChain->Present(0, 0);
}

void CGraphic_Device::Map_Picking_Screen()
{
	m_pDeviceContext->CopyResource(m_pStagingTexture, m_RenderTargets[strObjectIdRenderTargetTag]->Get_Texture());

	m_pDeviceContext->Map(m_pStagingTexture, 0, D3D11_MAP_READ, 0, &m_mappedResource);
}

void CGraphic_Device::Unmap_Picking_Screen()
{

	m_pDeviceContext->Unmap(m_pStagingTexture, 0);
}

_uint CGraphic_Device::Pick_Screen(_uint iX, _uint iY)
{
	UINT* data = reinterpret_cast<UINT*>(m_mappedResource.pData);
	int pixelIndex = (iY * m_iWinSizeX + iX);
	unsigned int id = data[pixelIndex];

	return id;
}

void CGraphic_Device::Map_Picking_Position()
{
	m_pDeviceContext->CopyResource(m_pStagingPosition, m_RenderTargets[strPositionRenderTargetTag]->Get_Texture());

	m_pDeviceContext->Map(m_pStagingPosition, 0, D3D11_MAP_READ, 0, &m_mappedPosition);
}

void CGraphic_Device::Unmap_Picking_Position()
{
	m_pDeviceContext->Unmap(m_pStagingPosition, 0);
}

_float4 CGraphic_Device::Pick_Position(_uint iX, _uint iY)
{
	_float4* data = reinterpret_cast<_float4*>(m_mappedPosition.pData);
	int pixelIndex = (iY * m_iWinSizeX + iX);
	_float4 pos = data[pixelIndex];

	return pos;
}



HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	/* 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태  */
	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/*텍스처(백버퍼)를 생성하는 행위*/
	SwapChain.BufferDesc.Width = iWinCX;
	SwapChain.BufferDesc.Height = iWinCY;

	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;

	/*스왑하는 형태*/
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = hWnd;
	SwapChain.Windowed = isWindowed;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* 백버퍼라는 텍스처를 생성했다. */
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;



	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}


HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;



	/* 내가 앞으로 사용하기위한 용도의 텍스쳐를 생성하기위한 베이스 데이터를 가지고 있는 객체이다. */
	/* 내가 앞으로 사용하기위한 용도의 텍스쳐 : ID3D11RenderTargetView, ID3D11ShaderResoureView, ID3D11DepthStencilView */
	ID3D11Texture2D* pBackBufferTexture = nullptr;

	/* 스왑체인이 들고있던 텍스처를 가져와봐. */
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;



	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	//ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	//TextureDesc.Width = iWinCX / 4;
	//TextureDesc.Height = iWinCY / 4;
	//TextureDesc.MipLevels = 1;
	//TextureDesc.ArraySize = 1;
	//TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//TextureDesc.SampleDesc.Quality = 0;
	//TextureDesc.SampleDesc.Count = 1;

	//TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	//TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
	//	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	//TextureDesc.CPUAccessFlags = 0;
	//TextureDesc.MiscFlags = 0;

	//if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
	//	return E_FAIL;

	///* RenderTarget */
	///* ShaderResource */
	///* DepthStencil */

	//if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilViewQuarter)))
	//	return E_FAIL;

	//Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_RenderTargets(_uint iWinCX, _uint iWinCY)
{

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = iWinCX;
	textureDesc.Height = iWinCY;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Add_RenderTarget(strAlbedoRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });
	//m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pAlbedoTexture);
	Add_RenderTarget(strNormalRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4{ 0.f,0.f,0.f,0.f });
	//m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pNormalTexture);
	Add_RenderTarget(strOrmRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4{ 0.f,0.f,0.f,0.f });
	//m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pOrmTexture);


	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Add_RenderTarget(strPositionRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4{ 0.f,0.f,0.f,0.f });
	//m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pPositionTexture);

	textureDesc.Format = DXGI_FORMAT_R32_UINT;
	Add_RenderTarget(strObjectIdRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R32_UINT, _float4{ 0.f,0.f,0.f,0.f });
	//m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pIDTexture);

	Add_RenderTarget(strEmissionRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });

	//Add_RenderTarget(strGlowRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });
	Add_RenderTarget(strGlowRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4{ 0.f,0.f,0.f,0.f });
	Add_RenderTarget(strGlowPowerRenderTargetTag, iWinCX / 4, iWinCY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4{ 0.f,0.f,0.f,0.f });
	Add_RenderTarget(strGlowBlurRenderTargetTag, iWinCX / 4, iWinCY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4{ 0.f,0.f,0.f,0.f });


	Add_RenderTarget(strBeforeBackTarget, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });
	Add_RenderTarget(strCrossPowerTarget, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });
	Add_RenderTarget(strDistortionRenderTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });


	Add_RenderTarget(strWeightedBlendedTargetTag, iWinCX, iWinCY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4{ 0.f,0.f,0.f,0.f });
	//Add_RenderTarget(strCrossUpTarget, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });
	//Add_RenderTarget(strCrossRightTarget, iWinCX, iWinCY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4{ 0.f,0.f,0.f,0.f });


	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.Format = DXGI_FORMAT_R32_UINT;
	m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pStagingTexture);

	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pStagingPosition);

	return S_OK;
}

HRESULT CGraphic_Device::Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strRenderTargetTag))
		return E_FAIL;

	shared_ptr<CRenderTarget> pRenderTarget = CRenderTarget::Create(m_pDevice, m_pDeviceContext, iWidth, iHeight, ePixel, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strRenderTargetTag, pRenderTarget);

	return S_OK;
}
HRESULT CGraphic_Device::Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag)
{

	shared_ptr<class CRenderTarget> pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<shared_ptr<class CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
	{
		list<shared_ptr<class CRenderTarget>>	MRTList;
		MRTList.emplace_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->emplace_back(pRenderTarget);

	//Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CGraphic_Device::Bind_RT_SRV(const _wstring& strRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	auto pRT = Find_RenderTarget(strRTTag);
	if (!pRT)
	{
		return E_FAIL;
	}

	return pRT->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CGraphic_Device::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool bClear)
{
	list<shared_ptr<class CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	ID3D11RenderTargetView* pMRTs[8] = {};

	_uint		iNumRenderTargets = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		if (bClear)
			pRenderTarget->Clear();
		pMRTs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}
	if (!pDSV)
		m_pDeviceContext->OMSetRenderTargets(iNumRenderTargets, pMRTs, m_pDepthStencilView);
	else
		m_pDeviceContext->OMSetRenderTargets(iNumRenderTargets, pMRTs, pDSV);
	return S_OK;
}

HRESULT CGraphic_Device::End_MRT()
{
	Bind_BackBuffer();

	//Safe_Release(m_pOldRTV);
	//Safe_Release(m_pDSV);

	return S_OK;
}

shared_ptr<class CRenderTarget> CGraphic_Device::Find_RenderTarget(const _wstring& strRenderTargetTag)
{
	auto	iter = m_RenderTargets.find(strRenderTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<shared_ptr<class CRenderTarget>>* CGraphic_Device::Find_MRT(const _wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}
HRESULT CGraphic_Device::Bind_Default_ViewPort()
{
	m_pDeviceContext->RSSetViewports(1, &m_ViewPort);
	return S_OK;
}
#ifdef _DEBUG

HRESULT CGraphic_Device::Ready_Debug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	auto pRT = Find_RenderTarget(strRenderTargetTag);
	if (!pRT)
		return E_FAIL;

	return pRT->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CGraphic_Device::Render_Debug(const _wstring& strMRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName, shared_ptr< class CVIBuffer_Rect> pVIBuffer)
{
	auto MRT = Find_MRT(strMRTTag);
	if (!MRT)
		return E_FAIL;

	for (auto& pRTV : *MRT)
	{
		pRTV->Render_Debug(pShader, pConstantName, pVIBuffer);
	}

	return S_OK;
}

#endif 


shared_ptr<CGraphic_Device> CGraphic_Device::Create(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContextOut)
{
	MAKE_SHARED_ENABLER(CGraphic_Device);
	shared_ptr<CGraphic_Device>  pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(EngineDesc, ppDevice, ppDeviceContextOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CGraphic_Device"));
		//Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	for (auto& Pair : m_MRTs)
	{
		//for (auto& pRenderTarget : Pair.second)
		//	Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();

	//for (auto& Pair : m_RenderTargets)
	//	Safe_Release(Pair.second);

	m_RenderTargets.clear();
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDepthTexture);
	Safe_Release(m_pHitScreenTexture);
	Safe_Release(m_pHitScreenRTV);

	Safe_Release(m_pStagingTexture);
	Safe_Release(m_pStagingPosition);

#if defined(DEBUG) || defined(_DEBUG)
	/*ID3D11Debug* d3dDebug;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	}
	if (d3dDebug != nullptr)            d3dDebug->Release();*/
#endif


	Safe_Release(m_pDevice);
}
