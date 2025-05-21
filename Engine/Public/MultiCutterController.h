#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMultiCutterController : public CComponent
{
public:
	struct CUTTER_DESC
	{
		CGameObject* pOwner = nullptr;
		wstring strModelComTag = L"";
		_float fApartAmount = 1.f;
	};


protected:
	CMultiCutterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMultiCutterController(const CMultiCutterController& rhs);
public:
	virtual ~CMultiCutterController();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	void Update(_float fTimeDelta);

	const vector<shared_ptr<class CModel>>& Get_Models() const { return m_Models; }
	const vector<vector<_uint>>& Get_ControlledMeshs() const { return m_ControlledMeshIdx; }
	const vector<pair<_float, _float>>& Get_Timer() const { return m_Timers; }
	void Cut(const _float4& plane);

	void Deactivate();
	void Activate();
	
	void ApplyAnimation(_uint iIdx, _float fTrackPos);
	void ApplyBones();

protected:
	vector<shared_ptr<class CModel>>     m_Models;
	vector<pair<_float, _float>>		 m_Timers;
	vector<shared_ptr<class CPxRagDoll>> m_RagDolls;
	vector<vector<shared_ptr<class CModel_Cutter>>> m_Computers;
	vector<vector<_uint>> m_ControlledMeshIdx;
	vector<vector<_float4>> m_Planes;

public:
	static shared_ptr<CMultiCutterController> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END