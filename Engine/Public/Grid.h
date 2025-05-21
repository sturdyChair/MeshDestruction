#pragma once
#include "Engine_Defines.h"
#include "NvFlowExt.h"
#include "NvFlowLoader.h"

BEGIN(Engine)


class CGrid
{
private:
	CGrid();

public:
	~CGrid();
public:
	HRESULT Initialize(shared_ptr<class CFlowManager> pManager, _uint iMaxLocation, const string& name);

	void Simulate(_float fTimeDelta);

private:
	NvFlowGrid*				grid = nullptr;
	NvFlowGridParamsNamed* gridParamsNamed = nullptr;
	weak_ptr<class CFlowManager> m_pManager;
	string m_name;
	_uint m_iMaxLocation = 0;
public:
	static shared_ptr<CGrid> Create(shared_ptr<class CFlowManager> pManager, _uint iMaxLocation, const string& name);
	void Free();


	friend class CFlowManager;
};

END