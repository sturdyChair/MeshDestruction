#include "..\Public\Light_Manager.h"
#include "Light.h"
#include "GameObject.h"
#include "Shader.h"

CLight_Manager::CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
}

CLight_Manager::~CLight_Manager()
{
	Free();
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_Lights.end())
		return nullptr;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::TickEngine(_float fTimeDelta)
{
	LightDeleteCheck();

	for (auto& pLight : m_Lights) {
		if (nullptr != pLight) {
			pLight->PriorityTick(fTimeDelta);
			pLight->Tick(fTimeDelta);
			pLight->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

//HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
//{
//	m_iLightCount = 0;
//
//	for (auto& pLight : m_Lights) {
//		if (nullptr != pLight) {
//			if (pLight->Get_LightState()) {
//				m_iLightCount++;
//				pLight->Lighting(pShader, pVIBuffer);
//			}
//		}
//	}
//	return S_OK;
//}

void CLight_Manager::LightControl(string strLightTag, _bool bOn)
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight) {
			if (strLightTag == pLight->Get_LightDesc()->szName)
				pLight->Set_LightState(bOn);
		}
	}
}

void CLight_Manager::LightControl(string strLightTag)
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight) {
			if (strLightTag == pLight->Get_LightDesc()->szName)
				pLight->Set_LightState(!pLight->Get_LightState());
		}
	}
}

void CLight_Manager::LightClear()
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight) {
			pLight->Dead();
		}
	}
}

void CLight_Manager::LightDeleteCheck()
{
	//for (auto& pLight : m_Lights)
	//{
	//	if (nullptr != pLight) {
	//		if (pLight->Get_IsDead())
	//			Safe_Release(pLight);
	//	}
	//}

	for (auto pDelete = m_Lights.begin(); pDelete != m_Lights.end();)
	{
		if ((*pDelete)->Is_Dead()) {
			pDelete = m_Lights.erase(pDelete);
		}
		else
			++pDelete;
	}
}

HRESULT CLight_Manager::Add_Light(void* LightDesc)
{
	shared_ptr<CLight> pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace_back(pLight);

	return S_OK;
}

shared_ptr<CLight> CLight_Manager::Add_Light_Return(void* LightDesc)
{
	shared_ptr<CLight> pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
	if (nullptr == pLight)
		assert(false);

	m_Lights.emplace_back(pLight);

	return pLight;
}

HRESULT CLight_Manager::Bind_Lights(shared_ptr<class CShader> pShader)
{
	m_pointLights.clear();
	m_directionalLights.clear();
	m_spotLights.clear();

	for (auto& pLight : m_Lights)
	{
		if(pLight->Get_LightState())
		{
			auto pDesc = pLight->Get_LightDesc();
			switch (pDesc->eType)
			{
			case LIGHT_DESC::TYPE_DIRECTIONAL:
			{
				DirectionalLight DLight{};
				DLight.ambientColor = pDesc->vAmbient;
				XMStoreFloat3(&DLight.color, XMLoadFloat4(&pDesc->vDiffuse));
				XMStoreFloat3(&DLight.direction, XMLoadFloat4(&pDesc->vDirection));
				DLight.intensity = pDesc->g_fIntensity;
				XMStoreFloat3(&DLight.specColor, XMLoadFloat4(&pDesc->vSpecular));
				m_directionalLights.push_back(DLight);
			}
			break;
			case LIGHT_DESC::TYPE_SPOT:
			{
				SpotLight SLight{};
				XMStoreFloat3(&SLight.color, XMLoadFloat4(&pDesc->vDiffuse));
				XMStoreFloat3(&SLight.direction, XMLoadFloat4(&pDesc->vDirection));
				SLight.innerConeCos = cos(pDesc->g_fSpotInnerConeAngle);
				SLight.intensity = pDesc->g_fIntensity;
				SLight.outerConeCos = cos(pDesc->g_fSpotOuterConeAngle);
				XMStoreFloat3(&SLight.position, XMLoadFloat4(&pDesc->vPosition));
				SLight.range = pDesc->fRange;
				XMStoreFloat3(&SLight.specColor, XMLoadFloat4(&pDesc->vSpecular));
				m_spotLights.push_back(SLight);
			}
			break;
			case LIGHT_DESC::TYPE_POINT:
			{
				PointLight PLight{};
				PLight.ambientColor = pDesc->vAmbient;
				XMStoreFloat3(&PLight.color, XMLoadFloat4(&pDesc->vDiffuse));
				PLight.intensity = pDesc->g_fIntensity;
				XMStoreFloat3(&PLight.position, XMLoadFloat4(&pDesc->vPosition));
				PLight.range = pDesc->fRange;
				XMStoreFloat3(&PLight.specColor, XMLoadFloat4(&pDesc->vSpecular));
				m_pointLights.push_back(PLight);
			}
			break;
			default:
				break;
			}
		}
	}

	_int iNumLight = static_cast<_int>(m_pointLights.size());
	if (iNumLight > 32)
		iNumLight = 32;
	pShader->Bind_RawValue("pointLights", m_pointLights.data(), iNumLight * sizeof(PointLight));
	pShader->Bind_RawValue("iNumPoint", &iNumLight, sizeof(_int));

	iNumLight = static_cast<_int>(m_directionalLights.size());
	if (iNumLight > 32)
		iNumLight = 32;
	pShader->Bind_RawValue("directionalLights", m_directionalLights.data(), iNumLight * sizeof(DirectionalLight));
	pShader->Bind_RawValue("iNumDirectional", &iNumLight, sizeof(_int));

	iNumLight = static_cast<_int>(m_spotLights.size());
	if (iNumLight > 32)
		iNumLight = 32;
	pShader->Bind_RawValue("spotLights", m_spotLights.data(), iNumLight * sizeof(SpotLight));
	pShader->Bind_RawValue("iNumSpot", &iNumLight, sizeof(_int));



	return S_OK;
}

shared_ptr<CLight_Manager> CLight_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLight_Manager
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLight_Manager(pDevice, pContext)
		{

		}
	};


	shared_ptr<CLight_Manager> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLight_Manager"));
		assert(false);
		//Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight_Manager::Free()
{

	m_Lights.clear();
}
