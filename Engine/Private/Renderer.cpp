#include "..\Public\Renderer.h"
#include "Graphic_Device.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext },
	m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice); Safe_AddRef(m_pContext);
}

CRenderer::~CRenderer()
{
	Free();
}

HRESULT CRenderer::Initialize()
{
	m_vResolution = m_pGameInstance.lock()->Get_Resolution();


	XMStoreFloat4x4(&m_FullScreenWorld, XMMatrixScaling(m_vResolution.x, m_vResolution.y, 1.f));
	XMStoreFloat4x4(&m_HalfScreenWorld, XMMatrixScaling(m_vResolution.x * 0.5f, m_vResolution.y * 0.5f, 1.f));
	XMStoreFloat4x4(&m_QuatScreenWorld, XMMatrixScaling(m_vResolution.x * 0.25f, m_vResolution.y * 0.25f, 1.f));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_vResolution.x, m_vResolution.y, 0.f, 1.f));;
	XMStoreFloat4x4(&m_ProjMatrixHalf, XMMatrixOrthographicLH(m_vResolution.x * 0.5f, m_vResolution.y * 0.5f, 0.f, 1.f));
	XMStoreFloat4x4(&m_ProjMatrixQuat, XMMatrixOrthographicLH(m_vResolution.x * 0.25f, m_vResolution.y * 0.25f, 0.f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (!m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Deferred.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);

	ID3D11Texture2D* pDepthStencilTexture = nullptr;



	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = static_cast<UINT>(m_vResolution.x * 0.25f);
	TextureDesc.Height = static_cast<UINT>(m_vResolution.y * 0.25f);
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

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilViewQuarter)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	ZeroMemory(&m_ViewPort, sizeof(D3D11_VIEWPORT));
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = m_vResolution.x * 0.25f;
	m_ViewPort.Height = m_vResolution.y * 0.25f;
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;



	return S_OK;
}

HRESULT CRenderer::Add_RenderObjects(RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);


	return S_OK;
}

HRESULT CRenderer::Draw()
{
	auto pGameInstance = m_pGameInstance.lock();
	m_pContext->ClearDepthStencilView(m_pDepthStencilViewQuarter, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_BeforeBack_Tag);
	if (FAILED(Render_Priority()))
		return E_FAIL;

	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_D_N_ORM_Tag);

	if (FAILED(Render_Terrain()))
		return E_FAIL;

	if (FAILED(Render_Decal()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_BeforeBack_Tag, nullptr, false);

	Render_Deferred();

	if (FAILED(Render_NonBlend_Non_Lit()))
		return E_FAIL;

	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_WeightedBlended_Tag);
	if (FAILED(Render_Blend_Non_Lit()))
		return E_FAIL;
	Render_WeightedBlended();
	
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_Glow_Tag);

	if (FAILED(Render_Glowing_Object()))
		return E_FAIL;

	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_BeforeBack_Tag, nullptr, false);

	Render_Glow();
	//Render_Cross();

	Render_Distortion();

	Render_To_BackBuffer();

	if (FAILED(Render_Lens()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

void CRenderer::Clear()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		RenderObjects.clear();
	}
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Terrain()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_TERRAIN])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_TERRAIN].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Decal()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_DECAL])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_DECAL].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}
	if (m_RenderObjects[RENDER_NONBLEND].size())
		GAMEINSTANCE->Voronoi_Debug_Draw();
	m_RenderObjects[RENDER_NONBLEND].clear();
	//GAMEINSTANCE->Voronoi_Debug_Draw();
	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend_Non_Lit()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND_NON_LIT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_BLEND_NON_LIT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend_Non_Lit()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND_NON_LIT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_NONBLEND_NON_LIT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Glowing_Object()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_GLOW])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_GLOW].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	auto pGameInstance = m_pGameInstance.lock();
	if (m_RenderObjects[RENDER_DISTORTION].size() == 0)
		return S_OK;
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_Distortion_Tag);

	for (auto& pRenderObject : m_RenderObjects[RENDER_DISTORTION])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_DISTORTION].clear();
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_BeforeBack_Tag, nullptr, false);

	pGameInstance->Bind_RT_SRV(CGraphic_Device::strDistortionRenderTargetTag, m_pShader, "g_Texture");

	m_pShader->Begin(12);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(12);

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Lens()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_LENS])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
	}

	m_RenderObjects[RENDER_LENS].clear();

	return S_OK;
}


HRESULT CRenderer::Render_To_BackBuffer()
{
	auto pGameInstance = m_pGameInstance.lock();
	pGameInstance->End_MRT();
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strBeforeBackTarget, m_pShader, "gAlbedo")))
		return E_FAIL;

	if (m_bDissolve)
	{
		m_bDissolve = false;
		if (m_pNoise_Texture)
		{
			m_pNoise_Texture->Bind_ShaderResource(m_pShader, "g_Noise_Texture");
		}
		m_pShader->Bind_Float("g_Dissolve", m_fDissolve);
		m_pShader->Bind_Float("g_DissolveRange", m_fDissolveRange);
		m_pShader->Begin(13);
		m_pVIBuffer->Render();
		m_pShader->Bind_SRV("gAlbedo", NULL);
		m_pShader->Begin(13);
	}
	else if (m_bBlur)
	{
		m_bBlur = false;
		m_pShader->Bind_RawValue("screenCenter", &m_vRadialBlurCenter, sizeof(_float2));
		m_pShader->Bind_Float("blurStrength", m_fBlurStrength);
		m_pShader->Begin(RADIAL_BLUR);
		m_pVIBuffer->Render();
		m_pShader->Bind_SRV("gAlbedo", NULL);
		m_pShader->Begin(RADIAL_BLUR);
	}
	else
	{
		m_pShader->Begin(1);
		m_pVIBuffer->Render();
		m_pShader->Bind_SRV("gAlbedo", NULL);
		m_pShader->Begin(1);

	}
	m_pNoise_Texture = nullptr;

	return S_OK;
}

void CRenderer::Blur_On(const _float2& vCenter, const _float& fStrength)
{
	m_vRadialBlurCenter = vCenter; m_fBlurStrength = fStrength; m_bBlur = true;
}

void CRenderer::Dissolve_On(shared_ptr<class CTexture> pNoiseTexture, const _float& fDissolve, const _float& fRange)
{
	m_bDissolve = true; m_fDissolve = fDissolve; m_fDissolveRange = fRange; m_pNoise_Texture = pNoiseTexture;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_RTDebug()
{
	auto pGameInstance = m_pGameInstance.lock();
	m_pShader->Bind_Matrix("g_ViewMatrix", &Identity);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	pGameInstance->Render_RTDebug(CGraphic_Device::strMRT_D_N_ORM_Tag, m_pShader, "g_Texture", m_pVIBuffer);

	return S_OK;
}
#endif
HRESULT CRenderer::Render_Deferred()
{
	/*
	gAlbedo;
	gNormal;
	gORM;
	gPosition;
	*/
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strAlbedoRenderTargetTag, m_pShader, "gAlbedo")))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strNormalRenderTargetTag, m_pShader, "gNormal")))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strOrmRenderTargetTag, m_pShader, "gORM")))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strPositionRenderTargetTag, m_pShader, "gPosition")))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strEmissionRenderTargetTag, m_pShader, "gEmission")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_FullScreenWorld)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &Identity)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	pGameInstance->Bind_Lights(m_pShader);
	_float4 vCamPos = pGameInstance->Get_CamPosition_Float4();
	m_pShader->Bind_Vector4("cameraPos", &vCamPos);
	if (FAILED(m_pShader->Bind_Float("texScalerX", 1.f / 128.f)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Float("texScalerY", 1.f / 72.f)))
		return E_FAIL;
	//_float4 vLightPos = { 10.f,20.f, 0.f,1.f };
	//m_pShader->Bind_Vector4("g_vLightPos", &vLightPos);
	static _uint iRenderPass = 2;
	//if (pGameInstance->Get_KeyDown(DIK_0))
	//{
	//	++iRenderPass;
	//	iRenderPass %= 5;
	//}
	//if (iRenderPass == 2)
	//{
		//m_pShader->Begin(14);
	//}
	//else
	//{
		m_pShader->Begin(iRenderPass);
	//}

	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("gAlbedo", NULL);
	m_pShader->Bind_SRV("gNormal", NULL);
	m_pShader->Bind_SRV("gORM", NULL);
	m_pShader->Bind_SRV("gPosition", NULL);
	m_pShader->Bind_SRV("gEmission", NULL);
	m_pShader->Begin(iRenderPass);

	return S_OK;
}
HRESULT CRenderer::Render_Glow()
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_FullScreenWorld)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &Identity)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Float("texScalerX", 1.f / 128.f)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Float("texScalerY", 1.f / 72.f)))
		return E_FAIL;

	m_pContext->RSSetViewports(1, &m_ViewPort);
#pragma region PASS_POWER
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_GlowPower_Tag, m_pDepthStencilViewQuarter);

	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strGlowRenderTargetTag, m_pShader, "g_Texture")))
		return E_FAIL;
	m_pShader->Begin(GP_POWER);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(GP_POWER);
#pragma endregion

#pragma region PASS_BLUR_X
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_GlowBlur_Tag, m_pDepthStencilViewQuarter);
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strGlowPowerRenderTargetTag, m_pShader, "g_Texture")))
		return E_FAIL;
	m_pShader->Begin(GP_X);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(GP_X);
#pragma endregion

#pragma region PASS_BLUR_Y
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_GlowPower_Tag, m_pDepthStencilViewQuarter);
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strGlowBlurRenderTargetTag, m_pShader, "g_Texture")))
		return E_FAIL;
	m_pShader->Begin(GP_Y);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(GP_Y);
#pragma endregion

	pGameInstance->Bind_Default_ViewPort();
#pragma region PASS_GLOW
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_BeforeBack_Tag, nullptr, false);

	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strGlowPowerRenderTargetTag, m_pShader, "g_GlowTexture")))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strGlowRenderTargetTag, m_pShader, "g_Texture")))
		return E_FAIL;
	m_pShader->Begin(GP_GLOW);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_GlowTexture", NULL);
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(GP_GLOW);
#pragma endregion



	return S_OK;
}
HRESULT CRenderer::Render_WeightedBlended()
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_FullScreenWorld)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &Identity)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_BeforeBack_Tag, nullptr, false);

	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strWeightedBlendedTargetTag, m_pShader, "g_Texture")))
		return E_FAIL;
	m_pShader->Begin(15);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(15);

	return S_OK;
}
HRESULT CRenderer::Render_Cross()
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_FullScreenWorld)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &Identity)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Float("texScalerX", 1.f / 128.f)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Float("texScalerY", 1.f / 72.f)))
		return E_FAIL;

#pragma region PASS_POWER
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_CrossPower_Tag);
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strBeforeBackTarget, m_pShader, "g_Texture")))
		return E_FAIL;
	m_pShader->Begin(CP_POWER);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(CP_POWER);
#pragma endregion
#pragma region PASS_CROSS
	pGameInstance->Begin_MRT(CGraphic_Device::strMRT_BeforeBack_Tag, nullptr, false);
	if (FAILED(pGameInstance->Bind_RT_SRV(CGraphic_Device::strCrossPowerTarget, m_pShader, "g_Texture")))
		return E_FAIL;
	m_pShader->Begin(CROSSPASS);
	m_pVIBuffer->Render();
	m_pShader->Bind_SRV("g_Texture", NULL);
	m_pShader->Begin(CROSSPASS);
#pragma endregion




	return S_OK;
}


shared_ptr<CRenderer> CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CRenderer
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRenderer(pDevice, pContext)
		{
		}
	};
	shared_ptr<CRenderer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		RenderObjects.clear();
	}

	Safe_Release(m_pDevice); Safe_Release(m_pContext);
	Safe_Release(m_pDepthStencilViewQuarter);
}
