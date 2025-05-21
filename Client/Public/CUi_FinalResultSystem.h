#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_FinalResultSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_FinalResultSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_FinalResultSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_FinalResultSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	void Create();


private:
	vector<shared_ptr<class CUi_FinalResult>> m_pResultVec;
	_float m_fCreateTimeGap = { 0 };
	_int m_iCount = { 0 };


public:
	static shared_ptr<CUi_FinalResultSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
