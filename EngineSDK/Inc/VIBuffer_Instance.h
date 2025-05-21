#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance : public CVIBuffer
{

public:
	enum INSTANCE_STYLE
	{
		STYLE_NONE,
		STYLE_SPREAD,
		STYLE_DROP,
		STYLE_FOUNTAIN,
		STYLE_RISING,
		STYLE_CYCLE,
		STYLE_ORBITAL,
		STYLE_END,
	};

public:
	struct INSTANCE_DESC
	{
		_float2 vScale{};//MINMAX
		_float2 vSpeed{};//MINMAX
		_float2 vLifeTime{};//MINMAX
		_float3 vTorque{};

		_float3 vCenter{};
		_float3 vRange{};
		_float3 vPivot{};

		_bool bLoop = false;
		_uint iNumInstance = 0;
	};

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);

public:
	virtual ~CVIBuffer_Instance();

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* desc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

	virtual HRESULT Bind_Buffer() override;
	virtual HRESULT Draw() override;

public:
	virtual void Update_Spread(_float fTimeDelta) {};
	virtual void Update_Drop(_float fTimeDelta) {};
	virtual void Update_Fountain(_float fTimeDelta) {};
	virtual void Update_Rising(_float fTimeDelta) {};
	virtual void Update_Cycle(_float fTimeDelta) {};
	virtual void Update_Orbital(_float fTimeDelta) {};
	virtual void Reset() {};

public:
	_float3 Get_Pivot() { return m_tDesc.vPivot; }
	void Set_Pivot(_float3 vPivot) { m_tDesc.vPivot = vPivot; }

	INSTANCE_STYLE Get_Style() { return m_eStyle; }
	void Set_Style(INSTANCE_STYLE eStyle) { m_eStyle = eStyle; }

	_float2 Get_Speed() { return m_tDesc.vSpeed; }
	void Speed_Renewal(_float _fXSpeed, _float _fYSpeed);

	_float3 Get_Range() { return m_tDesc.vRange; }
	void Range_Renewal(_float3 vRange);

	_float2 Get_Scale() { return m_tDesc.vScale; }
	void Scale_Renewal(_float2 _vScale);

	_bool Get_IsRandomize() { return m_bIsRandomize; }
	void Set_IsRandomize(_bool bIsRandomize) { m_bIsRandomize = bIsRandomize; }

protected:
	ID3D11Buffer* m_pVBInstance = nullptr;
	INSTANCE_DESC m_tDesc;
	_uint m_iInstanceVertexStride = 0;
	_uint m_iNumIndexPerInstance = 0;
	_float* m_pSpeeds = nullptr;
	_float4* m_vecInitPos = nullptr;
	_float3* m_vecTorque = nullptr;

	INSTANCE_STYLE m_eStyle = STYLE_NONE;
	_bool m_bIsRandomize = false;

public:
	virtual shared_ptr<CComponent> Clone(void* pArg) override = 0;
	void Free();

};

END