#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CFrustum 
{
private:
	CFrustum();
public:
	virtual ~CFrustum();

public:
	HRESULT Initialize();
	void Update();

	void Transform_ToLocalSpace(_fmatrix WorldMatrix);

	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRange);
	_bool isIn_LocalSpace(_fvector vLocalPos, _float fRange);


private:
	weak_ptr<class CGameInstance> m_pGameInstance;

private:
	_float3				m_vPoints[8] = {};
	_float3				m_vWorldPoints[8] = {};
	_float4				m_vWorldPlanes[6] = {};
	_float4				m_vLocalPlanes[6] = {};

private:
	void Make_Planes(const _float3* pPoints, _float4* pPlanes);

public:
	static shared_ptr<CFrustum> Create();
	void Free();
};

END