#include "Grid.h"
#include "FlowManager.h"

CGrid::CGrid()
{
}

CGrid::~CGrid()
{
}

HRESULT CGrid::Initialize(shared_ptr<class CFlowManager> pManager, _uint iMaxLocation, const string& name)
{
	m_pManager = pManager;
	m_name = name;
	m_iMaxLocation = iMaxLocation;
	pManager->MakeGrid(iMaxLocation, name, grid, gridParamsNamed);

	return S_OK;
}

void CGrid::Simulate(_float fTimeDelta)
{
	auto pManager = m_pManager.lock();
	NvFlowContext* context = pManager->GetContext();

	NvFlowGridParams* gridParams = pManager->loader.gridParamsInterface.mapGridParamsNamed(gridParamsNamed);

	NvFlowGridParamsDesc gridParamsDesc = {};
	NvFlowGridParamsSnapshot* paramsSnapshot = pManager->loader.gridParamsInterface.getParamsSnapshot(gridParams, fTimeDelta, 0llu);
	if (pManager->loader.gridParamsInterface.mapParamsDesc(gridParams, paramsSnapshot, &gridParamsDesc))
	{
		pManager->loader.gridInterface.simulate(
			context,
			grid,
			&gridParamsDesc,
			NV_FLOW_FALSE
		);

		pManager->loader.gridParamsInterface.unmapParamsDesc(gridParams, paramsSnapshot);
	}

	NvFlowUint64 flushedFrameID = 0llu;
	int deviceReset = pManager->loader.deviceInterface.flush(pManager->deviceQueue, &flushedFrameID, nullptr, nullptr);



	pManager->loader.deviceInterface.waitForFrame(pManager->deviceQueue, flushedFrameID);
	

	NvFlowUint64 lastCompletedFrame = pManager->contextInterface.getLastFrameCompleted(context);

}

shared_ptr<CGrid> CGrid::Create(shared_ptr<class CFlowManager> pManager, _uint iMaxLocation, const string& name)
{
	MAKE_SHARED_ENABLER(CGrid);
	shared_ptr<CGrid> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pManager, iMaxLocation, name)))
	{
		assert(false);
	}

	return pInstance;
}

void CGrid::Free()
{
}
