#include "..\Public\FlowManager.h"


#include "GameInstance.h"
#include <iostream>

void printError(const char* str, void* userdata)
{
	cout << str;
}



CFlowManager::CFlowManager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{

}

CFlowManager::~CFlowManager()
{
	Free();
}



void CFlowManager::Tick(_float fTimeDelta)
{

}

HRESULT CFlowManager::Initialize()
{
	NvFlowLoaderInit(&loader, printError, nullptr);

	if (!loader.module_nvflow || !loader.module_nvflowext)
	{
		printf("FlowLoader init() failed!!!\n");
		return E_FAIL;
	}
	// initialize compute/graphics
	{
		NvFlowBool32 validation = NV_FLOW_TRUE;

		deviceManager = loader.deviceInterface.createDeviceManager(validation, nullptr, 0u);

		NvFlowDeviceDesc deviceDesc = {};
		deviceDesc.deviceIndex = 0;
		deviceDesc.enableExternalUsage = false;
		deviceDesc.logPrint = NULL;

		device = loader.deviceInterface.createDevice(deviceManager, &deviceDesc);

		deviceQueue = loader.deviceInterface.getDeviceQueue(device);

		NvFlowContextInterface_duplicate(&contextInterface, loader.deviceInterface.getContextInterface(deviceQueue));
	}
	

	return S_OK;
}

NvFlowContext* CFlowManager::GetContext()
{
	return loader.deviceInterface.getContext(deviceQueue);;
}

void CFlowManager::MakeGrid(_uint iMaxLocation, const string& name, NvFlowGrid*& grid, NvFlowGridParamsNamed*& gridParamsNamed)
{
	NvFlowContext* context = GetContext();
	{
		NvFlowGridDesc gridDesc = NvFlowGridDesc_default;

		if (iMaxLocation > 0u)
		{
			gridDesc.maxLocations = iMaxLocation;
		}

		grid = loader.gridInterface.createGrid(&contextInterface, context, &loader.opList, &loader.extOpList, &gridDesc);
		gridParamsNamed = loader.gridParamsInterface.createGridParamsNamed(name.c_str());
	}

}

shared_ptr<CFlowManager> CFlowManager::Create()
{
	MAKE_SHARED_ENABLER(CFlowManager)

	auto pInstance = make_shared<MakeSharedEnabler>();

	pInstance->Initialize();

	return pInstance;
}

void CFlowManager::Free()
{
}


