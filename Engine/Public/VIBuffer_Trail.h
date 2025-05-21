

#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail : public CVIBuffer
{
public:
	struct TRAIL_DESC
	{
		_uint iMaxRect = 0;
	};


private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
public:
	virtual ~CVIBuffer_Trail();

public:
	virtual HRESULT Initialize_Prototype(const TRAIL_DESC& desc);
	virtual HRESULT Initialize(void* pArg) override;

	void Start(_float4x4* pUpper, _float4x4* pDown);
	void Tick(_float fTimeDelta);
	void End();
private:
	void Update_Buffer();
	void SetUp_Buffer();

private:
	_uint m_iStartingPoint = 0;
	_uint m_iMaxRect = 0;

	_bool m_bOn = false;
	_bool m_bStart = false;
	_float m_fTimer = 0.f;
	_float4x4* m_pUpperTransform = nullptr;
	_float4x4* m_pDownTransform = nullptr;
	deque<_float3> m_vecUpperPositions;
	deque<_float3> m_vecDownPositions;

public:
	static shared_ptr<CVIBuffer_Trail> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_DESC& desc);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END