#pragma once

#include "Engine_Defines.h"
#include "NvFlowExt.h"
#include "NvFlowLoader.h"

BEGIN(Engine)

class CFlowManager : public enable_shared_from_this<CFlowManager>
{
private:
	CFlowManager();
public:
	virtual ~CFlowManager();

public:
	void Tick(_float fTimeDelta);

	HRESULT Initialize();

	NvFlowContext* GetContext();

	void MakeGrid(_uint iMaxLocation, const string& name, NvFlowGrid*& grid, NvFlowGridParamsNamed*& gridParamsNamed);


private:
	map<_wstring, shared_ptr<class CGrid>> m_mapGrids;

	weak_ptr<class CGameInstance> m_pGameInstance;

	NvFlowLoader loader = {};
	NvFlowContextInterface contextInterface = {};

	NvFlowDeviceManager* deviceManager = nullptr;
	NvFlowDevice* device = nullptr;
	NvFlowDeviceQueue* deviceQueue = nullptr;



public:
	static shared_ptr<CFlowManager> Create();
	void Free();


	friend class CGrid;
};

END