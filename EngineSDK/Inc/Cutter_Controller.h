#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCutter_Controller : public CComponent
{
public:
	struct CUTTER_DESC
	{
		CGameObject* pOwner = nullptr;
		wstring strModelComTag = L"";
		_float fApartAmount = 1.f;
	};


protected:
	CCutter_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutter_Controller(const CCutter_Controller& rhs);
public:
	virtual ~CCutter_Controller();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	void Update(_float fTimeDelta);

	void Cut(const _float4& plane);

	const vector<_float3>& Get_Offsets() const;
	const vector<_float>& Get_LifeTimes() const;
	_float Get_MaxLifeTime()const {return m_fMaxLifeTime;}
protected:
	
	shared_ptr<class CModel>     m_pModel = nullptr;
	vector<shared_ptr<class CModel_Cutter>> m_Computers;
	vector<_float3> m_Offsets;
	vector<_float3> m_RealOffsets;
	vector<_uint>  m_Recursions;
	vector<_float>  m_LifeTimes;
	_uint m_iMaxRecursion = 5;
	_float m_fMaxLifeTime = 1.f;
	_float m_fApartAmount = 1.f;
	_float m_fOffsetInterpolTime = 2.f;
public:
	static shared_ptr<CCutter_Controller> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END